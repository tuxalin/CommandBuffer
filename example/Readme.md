# ThreadedRenderingGL Example
Based on NVIDIA's Gameworks GL Threading example, it was adapted to a deferred renderer with multiple volumetric lights to exemplify command buffer usage. 

It shows more advanced usage of how to use commands recording, pod hints, creating a material binder, prioritize custom commands, dispatch and have multiple command buffers for a deferred renderer.

![pic-main](screenshot.PNG)

## Usage
Requires Visual Studio 2017. 
You can click on the top left arrow to open the example settings menu.

## Breakdown
The example is composed of several worker theads that all issue GL commands, there are n-1 animation threads that issue vbo update commands for the fish animation and also draw commands. And also an additional thread for issuing defferred commands 

There are several buffer modes for async updates(recommended to also check the original NVIDIA example which has more modes):
```cpp
    enum
    {
        TECHNIQUE_GLES_SUBRANGE = 0,
        TECHNIQUE_GLES_SUBRANGE_PERSISTENT,
        TECHNIQUE_GLES_ORPHANED,
        TECHNIQUE_GLES_POOLED,
        TECHNIQUE_GLES_POOLED_PERSISTENT,
        TECHNIQUE_COUNT
    };  
```

### Material Binder
In order to reduce state changes commands are sorted according to their material id.
Thus you'll need to create a material binder that will handle the binding of materials, see Buffers.h.
A material will be defined as:
```cpp
    struct Material
    {
        std::string name;
        GLuint location = 0;
        GLuint ubo = 0;
        NvGLSLProgram* shader = nullptr;
    };
```

And the bind operator must be implemented to the Material Binder:
```cpp
    ///@note Returns true if there are more passes to bind.
    CB_FORCE_INLINE bool operator()(cb::MaterialId material) const
    {
        if (material.id == 0) // first material is a dummy, nothing to bind
            return false;

        const Material&  mat = materials[material.id];
        assert(mat.shader);
        if (mat.shader != activeShader)
        {
            //bind only if different shaders
            mat.shader->enable();
            activeShader = mat.shader;
        }
        if (material.id != activeMaterial)
        {
            // bind material ubo
            glBindBufferBase(GL_UNIFORM_BUFFER, mat.location, mat.ubo);
            activeMaterial = material.id;
        }

        return false;
    }
```

In the above case the material pass is ignored, which gets incremented in the command buffer submit, however if a material has more passes you will need to return true until all material passes are complete.

### Command buffers
There are 3 command buffers, see Buffers.h:
```cpp
typedef cb::CommandBuffer<cb::DrawKey, cb::DefaultKeyDecoder, Nv::MaterialBinder> GeometryCommandBuffer;
typedef cb::CommandBuffer<uint32_t,cb::DummyKeyDecoder<uint32_t>> DeferredCommandBuffer;
typedef cb::CommandBuffer<uint16_t,cb::DummyKeyDecoder<uint16_t>> PostProcessCommandBuffer;
```

The GeometryCommandBuffer will be used to fill the GBuffer, thus the geometry pass.
While the deferred pass(see ThreadedRenderingGL::DrawPointLightCommand) will be handled by the DeferredCommandBuffer with additive blending enabled when the pass begins.
Finally post processing, like volumetric lights(see ThreadedRenderingGL::PostProcessVolumetricLight), will be handled by the PostProcessCommandBuffer.

Before submitting commands they first be sorted, also a custom sorting functor can be passed:
```cpp
    m_geometryCommands.sort(radixsort<GeometryCommandBuffer::command_t>);
    m_deferredCommands.sort();
    m_postProcessCommands.sort();
```
NOTE. Radix sort is recommend for geometry commands as it will usually be faster for sorting integers.

A custom logger can be set via:
```cpp
    m_geometryCommands.setLogFunction(&commandLogFunction);
    m_deferredCommands.setLogFunction(&commandLogFunction);
    m_postProcessCommands.setLogFunction(&commandLogFunction);
```

### Comands
Commands can be private, inside a class like ThreadedRenderingGL::InitializeCommand(see ThreadedRenderingGL.h for more) or can be public like cmds::WaitFenceCommand(see Commands.h).
Drawing commands can be found at NvInstancedModelExtGL.h: RenderNonInstanced, RenderInstanced, RenderInstancedUpdate and UpdateVertexBinder.

For setting up kDispatchFunction member you can do it from a static function or from a command member via cb::makeExecuteFunction:
```cpp
    // via a static method
    const cb::RenderContext::function_t WaitFenceCommand::kDispatchFunction = &waitFenceCommand;

    // or via member and helper
    const cb::RenderContext::function_t NvInstancedModelExtGL::RenderNonInstanced::kDispatchFunction = &cb::makeExecuteFunction<NvInstancedModelExtGL::RenderNonInstanced>;

```

Commands that have non-POD members will required a hint as the linear allocator won't call the ctr:
```cpp
    struct DrawPointLightCommand
    {
        static const cb::RenderContext::function_t kDispatchFunction;
        // hint that we dont care about ctr/dtr
        typedef void pod_hint_tag;
        
        LightingUBO lightingUBO_Data; // non-pod member
        ...
```

### Command keys
For the command keys the default cb::DrawKey is used.
Custom commands are issued before executing the draw commands, like upading the VBOs for the schools:
```cpp
    // in ThreadedRenderingGL.cpp:454
    cb::DrawKey key = cb::DrawKey::makeCustom(cb::ViewLayerType::eHighest, 10);
    auto& cmd = *m_geometryCommands.addCommand<cmds::VboPoolUpdateCommand>(key);
    // in School.cpp:665
    cb::DrawKey key = cb::DrawKey::makeCustom(cb::ViewLayerType::eHighest, 10);
    auto& cmd = *geometryCommands.addCommand<cmds::VboUpdate>(key);
```

Each school will have it's own material:
```cpp
    int materialId = schoolIndex + 1; // first material is reserved
    pSchool->SetMaterial(cb::TranslucencyType::eOpaque, materialId);
    auto& mat = m_geometryCommands.materialBinder().materials[materialId];
    mat.name = "Fish school:";
    mat.name += std::to_string(schoolIndex);
    mat.shader = m_shader_Fish;
    mat.ubo = ubo.first;
    mat.location = ubo.second;
```

To take advantage of early-z culling opaque draw calls will be sorted in front to back order:
```cpp
    // draw front to back order for early-z culling
    nv::vec4f position = projView * nv::vec4f(m_lastCentroid.x, 1.f);
    float invDepth = (1.f - position.z / position.w);
    invDepth *= 10000.f;
    m_pInstancedModel->DrawKey().setDepth(invDepth);
```

### Command recording
Since we copy in the command all the data required from issuing commands we can reuse the previous submited commands and not clear the command buffers:
```cpp
    const bool clearCommands = !m_animPaused; // using recorded commands when paused
    // nothing to pass as GL doesn't have any contexts
    m_geometryCommands.submit(nullptr, clearCommands);
    m_deferredCommands.submit(nullptr, clearCommands);
    m_postProcessCommands.submit(nullptr, clearCommands);
```

Also because the VBOs dont change while the animations are paused when recording we exclude the vbo update commands: 
```cpp
    if (m_forceUpdateMode != ForceUpdateMode::eForceDispatch)
    {
        m_schools[i]->Animate(getClampedFrameTime(), &m_schoolStateMgr, m_avoidance);
        // Dispatch vbo update commands
        m_schools[i]->Update(m_geometryCommands);
    } 
```

But you can also use other command buffers only for VBO updates that would simplify the recording logic. 

## License

The code is available under a custom license as it based on [NVIDIA's Gameworks Samples](https://github.com/NVIDIAGameWorks/GraphicsSamples), see license.txt.
Code that is not part of the Gameworks example is avaiable under the MIT license.

# Lock-free CommandBuffer
A lock-free CommandBuffer implementation designed for multi-threaded rendering applications. 
Useful to reduce state switches(i.e. materials, shaders), depth sorting(for transparency or hardware early-z rejection) and also for dispatching commands from multiple threads.
## Features
- designed for high-congestion as it's lock-free
- render API indepdent(see cb::RenderContext)
- fast and configurable allocation via a linear allocator 
- optional material binder with multiple material passes support
- chainable/appendable commands
- configurable key type for sorting commands(opaque, transparent, depth sorting)
- easy to use and configurable draw key via bitfields
- debug utilities, tag commands
- lightweight, header only
- C++11 support
	
## Installation

The implementation is header only, it's only requirement is at least (C++11) support.

## Usage

Creating a command:
```cpp
    struct DrawArrays {
        static const cb::RenderContext::function_t kDispatchFunction;
        GLuint          vao;
        uint32_t        base;
        uint32_t        count;
        GLenum          primitive;
    };
    //in cpp
    void drawArrays(const void* data, cb::RenderContext*) {
        auto& cmd = *reinterpret_cast<const DrawGroundCommand*>(data);
        glBindVertexArray(cmd.vao);
        glDrawArrays(cmd.primitive, cmd.base, cmd.count);
    }
    const cb::RenderContext::function_t DrawArrays::kDispatchFunction = &drawArrays;
``` 
Dispatching a command:
```cpp
    //create the command key first
    cb::DrawKey key(0);
    key.setViewLayer(cb::ViewLayerType::e3D, cb::TranslucencyType::eOpaque);
    key.setDepth(depth);
    key.setMaterial(materialId);
    
    //then create command which will dispatch it
    cmds::DrawArrays& cmd = *commandBuffer.addCommand<cmds::DrawArrays>(key);
    cmd.vao = vaoId;
    cmd.base = 0;
    cmd.count = 4;
    cmd.primitive = GL_TRIANGLE_STRIP;
``` 	
Tagging a command for debug purposes, use CB_DEBUG_TAG_COMMANDS in config.h to enable/disable tagging:
```cpp
void testFunction() {
    cb::DrawKey key = cb::DrawKey::makeDefault(viewportId, cb::ViewLayerType::e3D);
    cmds::DrawArrays& cmd = *commandBuffer.addCommand<cmds::DrawArrays>(key);
    ...
    // will name tag: 'testFunction : <cpp_line_number> : struct cmds::DrawArrays * __ptr64'
    CB_DEBUG_COMMAND_TAG(cmd);
    // will name tag: 'testFunction : <cpp_line_number> : struct cmds::DrawArrays * __ptr64 : draw quad'
    CB_DEBUG_COMMAND_TAG_MSG(cmd, "draw quad");
    // will name tag: 'draw quad'
    CB_DEBUG_COMMAND_SET_MSG(cmd, "draw quad");
}
```
NOTE. Can enable/disable logging of the commands via CB_DEBUG_COMMANDS_PRINT in config.h which is enabled by default. 

Appending/chaining commands(useful to reduce overhead of redundant material binding):
```cpp
    cb::DrawKey key = cb::DrawKey::makeDefault(viewportId, cb::ViewLayerType::e3D);
    //setup key
    cmds::DrawArrays* cmd = commandBuffer.addCommand<cmds::DrawArrays>(key);
    //fill command data
    CB_DEBUG_COMMAND_SET_MSG(cmd, "draw first quad");
    
    //append multiple draw calls
    for(int i = 1; i < 10; ++i) {
        cmd = commandBuffer.appendCommand<cmds::DrawArrays>(cmd);
        //fill command data
        CB_DEBUG_COMMAND_SET_MSG(cmd, FORMAT_STR("draw quad %d", i));
    ...
``` 

Sometimes you may want to share/reference a command so you don't copy it's data multiple times:
```cpp
    //create the first the shared command
    CommandPacket* headerPacket = buffer.createCommandPacketData<cmds:SetMatrixCommand>(matrices);

    //reference/share the command
     key = cb::DrawKey::makeDefault(viewportId, cb::ViewLayerType::e3D);
    CommandPacket* cmd = buffer.addCommandFrom(key, headerPacket);
    //append a new command to the shared one
    cmds::DrawArrays* draw = buffer.appendCommand<cmds::DrawArrays>(cmd);
    //fill command data

    //reference it again
    key = cb::DrawKey::makeDefault(otherViewportId, cb::ViewLayerType::e3D);
    cmd = buffer.addCommandFrom(key, headerPacket);
    draw = buffer.appendCommand<cmds::DrawArrays>(cmd);
    //fill command data
    ...
``` 
NOTE. Since matrices is a POD you can use data copy commands which will automatically allocate auxilarry memory and copy it.

## Example

Check the example folder which shows how to use the CommandBuffers in a real use case, it was done by adapdting NVIDIA's Gameworks GL Threading example. 
It shows more advanced usage of how to create and use reference/shared commands, pod hints, creating a material binder, prioritize custom commands, dispatch and have multiple command buffers for a deferred renderer.

## Contributing

The implementation is mainly based on Stefan Reinalter's blog post at Molecular Matters, about a 'Stateless, layered, multi-threaded rendering' which I highly recommend reading.  

References:
- [Stateless, layered, multi-threaded rendering](https://blog.molecular-matters.com/2014/11/06/stateless-layered-multi-threaded-rendering-part-1/) by Stefan Reinalter
- [Order your graphics draw calls around!](http://realtimecollisiondetection.net/blog/?p=86) by Christer Ericson

Bug reports and pull requests are welcome on GitHub at https://github.com/tuxalin/CommandBuffer.

## License

The code is available as open source under the terms of the [MIT License](http://opensource.org/licenses/MIT).
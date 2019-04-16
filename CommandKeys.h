//
//  CommandKeys.h
//

#pragma once

#include <cassert>
#include <cstdint>
#ifndef NDEBUG
#include "MemoryUtil.h"
#endif

namespace cb
{
	enum class ViewLayerType {
		e3D = 0,
		eSkybox,
		eHighest = eSkybox + 1
	};

	enum class TranslucencyType
	{
		eSubtractive = 0,
		eAdditive,
		eNormal,
		eOpaque
	};

	/// Decoded material info.
	struct MaterialId
	{
		inline MaterialId(uint32_t id, uint32_t pass)
			: id(id)
			, pass(pass)
		{
		}

		/// Material id to bind.
		uint32_t id;
		/// The pass number of material.
		uint32_t pass;
	};

#pragma pack(push, 1)
	///@brief The key class for draw commands used for sorting and decoding the commands.
	struct DrawKey
	{
		static const uint64_t kOpaqueModeMask = 0x1F80000000000000;
		static const uint64_t kOpaqueModeValue = 0x0B00000000000000;
		static const uint32_t kPriorityBits = 7;

		/// The transparent mode for 3d translucent geometry.
		struct Transparent
		{
			/// The pass number of material.
			uint64_t materialPass : 8;
			/// Material id to bind.
			uint64_t materialId : 23;
			/// The depth order of a command, back to front.
			uint64_t depth : 24;
			/// Custom command bit, 0 for normal commands otherwise using Custom command mode.
			uint64_t cmd : 1;

		uint64_t: 3 + 3 + 2; // padding
		};

		/// The normal mode for opaque 3d geometry where the ordered of the material and depth is swapped.
		/// @note The reason for the swapped depth is to minimize state changes ahead of early Z culling.
		struct Opaque
		{
			/// The depth order of a command for opaque 3d geometry, front to back order.
			uint64_t depth : 24;
			/// The pass number of material.
			uint64_t materialPass : 8;
			/// Material id to bind.
			uint64_t materialId : 23;
			/// Custom command bit, 0 for normal commands otherwise using Custom command mode.
			uint64_t cmd : 1;

		uint64_t: 3 + 3 + 2; // padding
		};

		/// Custom command mode, can be use to add custom commands to switch a render target, clear buffers and
		/// other.
		struct Custom
		{
			/// Dummy bits, not used.
			uint64_t dummy : 48;
			/// The priority(sequence order) number of a custom command.
			uint64_t priority : kPriorityBits;
			/// Custom command bit, 0 for normal commands otherwise using custom Command mode.
			uint64_t enabled : 1;

		uint64_t: 3 + 3 + 2; // padding
		};

		union {
			struct
			{
			uint64_t: 64 - (3 + 3 + 2); ; // padding

				/// The translucency type of a command, @see cb::TranslucencyType
				uint64_t translucency : 2;
				/// The view layer type, @see cb::ViewLayerType
				uint64_t viewLayer : 3;
				/// The viewport id.
				uint64_t viewportId : 3;
			};
			/// Transparent command settings for 3d geometry.
			Transparent transparent;
			/// Normal opaque command settings, same as previous but with material and depth swapped.
			Opaque opaque;
			/// Custom command settings.
			Custom custom;

			uint64_t value;
		};

		DrawKey();
		explicit DrawKey(uint64_t value);
		explicit DrawKey(cb::ViewLayerType viewLayer);
		DrawKey(cb::ViewLayerType viewLayer, cb::TranslucencyType translucency);

		void setView(uint32_t viewportId, cb::ViewLayerType viewLayer, cb::TranslucencyType translucency);
		void setViewLayer(cb::ViewLayerType viewLayer, cb::TranslucencyType translucency);

		void setMaterial(uint32_t materialId);
		void setDepth(uint32_t depth);
		void setMaterialDepth(uint32_t materialdId, uint32_t depth);

		///@return The actual material based on the active mode, opaque or transparent.
		cb::MaterialId material() const;
		bool isOpaqueMode() const;

		///@note Used for sorting commands based on their precedence order.
		bool operator<(DrawKey other) const;

		explicit operator uint64_t();

		static DrawKey makeDefault(cb::ViewLayerType viewLayer);
		static DrawKey makeDefault(uint32_t viewportId, cb::ViewLayerType viewLayer = ViewLayerType::e3D);
		///@note The priority is inversed, i.e. lower values have highest priority.
		static DrawKey makeCustom(cb::ViewLayerType viewLayer, uint32_t priority);
		static DrawKey makeCustom(uint32_t viewportId, cb::ViewLayerType viewLayer, uint32_t priority);

		static void sanityChecks();
	};  // struct DrawKey
#pragma pack(pop)

	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	inline DrawKey::DrawKey()
	{
		static_assert(sizeof(DrawKey) == sizeof(uint64_t), "INVALID_DRAW_KEY_SIZE");
	}

	inline DrawKey::DrawKey(uint64_t value)
		: value(value)
	{
		this->viewLayer = static_cast<uint32_t>(viewLayer);
	}

	inline DrawKey::DrawKey(cb::ViewLayerType viewLayer)
		: value(0)
	{
		this->viewLayer = static_cast<uint32_t>(viewLayer);
	}

	inline DrawKey::DrawKey(cb::ViewLayerType viewLayer, cb::TranslucencyType translucency)
	{
		setViewLayer(viewLayer, translucency);
	}

	inline void DrawKey::setView(uint32_t viewportId, ViewLayerType viewLayer, cb::TranslucencyType translucency)
	{
		this->viewportId = viewportId;
		this->viewLayer = static_cast<uint32_t>(viewLayer);
		this->translucency = static_cast<uint32_t>(translucency);
	}

	inline void DrawKey::setViewLayer(ViewLayerType viewLayer, cb::TranslucencyType translucency)
	{
		this->viewLayer = static_cast<uint32_t>(viewLayer);
		this->translucency = static_cast<uint32_t>(translucency);
	}

	inline void DrawKey::setMaterial(uint32_t materialId)
	{
		if (isOpaqueMode())
			opaque.materialId = materialId;
		else
			// same memory layout as normal
			transparent.materialId = materialId;
	}

	inline void DrawKey::setDepth(uint32_t depth)
	{
		if (isOpaqueMode())
			opaque.depth = depth;
		else
			transparent.depth = depth;
	}

	inline void DrawKey::setMaterialDepth(uint32_t materialdId, uint32_t depth)
	{
		if (isOpaqueMode())
		{
			opaque.depth = depth;
			opaque.materialId = materialdId;
		}
		else
		{
			transparent.depth = depth;
			transparent.materialId = materialdId;
		}
	}

	inline cb::MaterialId DrawKey::material() const
	{
		return isOpaqueMode() ? cb::MaterialId(opaque.materialId, opaque.materialPass)
			: cb::MaterialId(transparent.materialId, transparent.materialPass);
	}

	inline bool DrawKey::isOpaqueMode() const
	{
		uint64_t keyValue = reinterpret_cast<const uint64_t&>(*this);
		return (keyValue & kOpaqueModeMask) == kOpaqueModeValue;
	}

	inline DrawKey::operator uint64_t()
	{
		return value;
	}

	inline bool DrawKey::operator<(DrawKey other) const
	{
		return value > other.value;
	}

	inline DrawKey DrawKey::makeDefault(cb::ViewLayerType viewLayer)
	{
		DrawKey key(0);
		key.viewLayer = static_cast<uint32_t>(viewLayer);
		return key;
	}

	inline DrawKey DrawKey::makeDefault(uint32_t viewportId, cb::ViewLayerType viewLayer)
	{
		DrawKey key(0);
		key.viewportId = viewportId;
		key.viewLayer = static_cast<uint32_t>(viewLayer);
		return key;
	}

	inline DrawKey DrawKey::makeCustom(cb::ViewLayerType viewLayer, uint32_t priority)
	{
		DrawKey key(viewLayer);
		key.custom.enabled = true;
		key.custom.priority = (2 << kPriorityBits) - 1 - priority;
		return key;
	}

	inline DrawKey DrawKey::makeCustom(uint32_t viewportId, cb::ViewLayerType viewLayer, uint32_t priority)
	{
		DrawKey key(0);
		key.viewportId = viewportId;
		key.viewLayer = static_cast<uint32_t>(viewLayer);
		key.custom.enabled = true;
		key.custom.priority = (2 << kPriorityBits) - 1 - priority;
		return key;
	}

	inline void DrawKey::sanityChecks()
	{
		// sanity checks for little-endian platforms
		assert(cb::mem::isLittleEndian());

		DrawKey drawKey;
		drawKey.value = 0;
		drawKey.viewportId = 3;
		drawKey.viewLayer = 0x2;
		drawKey.translucency = (uint32_t)TranslucencyType::eNormal; // = 0x2
		drawKey.custom.enabled = 0;
		drawKey.transparent.depth = 0x1234;
		drawKey.transparent.materialId = 0x5678;
		drawKey.transparent.materialPass = 5;
		assert(drawKey.value == 0x6a00091A00567805);
		// material and depth must be swapped
		drawKey.opaque.depth = 0x1234;
		drawKey.opaque.materialId = 0x5678;
		drawKey.opaque.materialPass = 5;
		assert(drawKey.value == 0x6a00567805001234);
		// command mode check
		drawKey.custom.enabled = 1;
		drawKey.custom.priority = 9;
		drawKey.custom.dummy = 0;
		assert(drawKey.value == 0x6a89000000000000);

		// masks for opaque mode checks
		drawKey.value = 0;
		drawKey.translucency = 0x3;
		drawKey.viewLayer = 0x7;
		drawKey.custom.enabled = 1;
		assert(drawKey.value == DrawKey::kOpaqueModeMask);
		drawKey.custom.enabled = 0;
		drawKey.translucency = (uint32_t)TranslucencyType::eOpaque; // = 0x3
		drawKey.viewLayer = 0x2;
		assert(drawKey.value == DrawKey::kOpaqueModeValue);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	inline const char* toString(cb::ViewLayerType type)
	{
		switch (type)
		{
		case cb::ViewLayerType::e3D:
			return "3D";
		case cb::ViewLayerType::eSkybox:
			return "Skybox";
		case cb::ViewLayerType::eHighest:
			return "Highest";
		default:
			assert(false);
			return "Unknown";
		}
	}

	inline const char* toString(cb::TranslucencyType type)
	{
		switch (type)
		{
		case cb::TranslucencyType::eSubtractive:
			return "Subtractive";
		case cb::TranslucencyType::eAdditive:
			return "Additive";
		case cb::TranslucencyType::eNormal:
			return "Normal";
		case cb::TranslucencyType::eOpaque:
			return "Opaque";
		default:
			assert(false);
			return "Unknown";
		}
	}

	inline std::ostream& operator<<(std::ostream& stream, cb::DrawKey key)
	{
		stream << key.viewportId << ", layer: " << cb::toString(cb::ViewLayerType(key.viewLayer)) << "-"
			<< cb::toString(cb::TranslucencyType(key.translucency)) << ", ";

		if (key.custom.enabled)
			stream << "custom command, priority: " << key.custom.priority;
		else if (cb::TranslucencyType(key.translucency) != cb::TranslucencyType::eOpaque)
			stream << ", depth: " << key.transparent.depth << ", material id: " << key.transparent.materialId;
		else if (key.isOpaqueMode())
			stream << ", depth: " << key.opaque.depth << ", material id: " << key.opaque.materialId;
		return stream;
	}
} // namespace cb

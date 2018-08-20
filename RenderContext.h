//
//  RenderContext.h
//

#pragma once

namespace cb
{
	struct RenderContext
	{
		typedef void(*function_t)(const void* command, cb::RenderContext* rc);

		RenderContext(void* contextData);

		template <class ContextClass>
		ContextClass& data();
		template <class ContextClass>
		const ContextClass& data() const;
	private:
		void* m_contextData;
	};

	inline RenderContext::RenderContext(void* contextData)
		: m_contextData(contextData)
	{}

	template <class ContextClass>
	inline ContextClass& RenderContext::data()
	{
		return *reinterpret_cast<ContextClass*>(m_contextData);
	}

	template <class ContextClass>
	inline const ContextClass& RenderContext::data() const
	{
		return *reinterpret_cast<const ContextClass*>(m_contextData);
	}
}


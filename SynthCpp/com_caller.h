#pragma once
#include <atlmem.h>
#include <ATLComMem.h>
#include <stdexcept>

class com_caller final
{
public:
	template<class TInterface, class TMethod, class... TArgs>
	static HRESULT call_method(CComPtr<TInterface> target, TMethod method, TArgs... arguments)
	{
		const HRESULT result = do_call(target, method, std::forward<TArgs>(arguments)...);

		if(FAILED(result))
		{
			throw std::runtime_error("Failed to execute com method");
		}

		return result;
	}

private:
	template<class TInterface, class TMethod, class... TArgs>
	static HRESULT do_call(CComPtr<TInterface> target, TMethod method, TArgs... arguments)
	{
		return ((target.p)->*method)(std::forward<TArgs>(arguments)...);
	}
};


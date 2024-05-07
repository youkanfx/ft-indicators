#include "pch.h"
#include <windows.h>
#include <malloc.h>
#include "StrategyInterfaceUnit.h"
#include "TechnicalFunctions.h"
#include "Cafeaulait.h"

namespace Technical {
	Cafeaulait::Cafeaulait(double rootPrice, double topPrice)
	{
		_topPrice = topPrice;
		_rootPrice = rootPrice;
		_state = Exploded;
	}

	void Cafeaulait::ModifyState(CafeaulaitState state)
	{
		_state = state;
	}

	CafeaulaitState Cafeaulait::GetState()
	{
		return _state;
	}

	double Cafeaulait::GetRootPrice() {
		return _rootPrice;
	}

	double Cafeaulait::GetTopPrice() {
		return _topPrice;
	}
}
#pragma once

namespace Technical {
	enum CafeaulaitState {
		Exploded,
		MiddleTouch,
		Entried,
	};

	class Cafeaulait {
	private:
		double _rootPrice;
		double _topPrice;
		CafeaulaitState _state;
	public:
		Cafeaulait(double rootPrice, double topPrice);
		void ModifyState(CafeaulaitState state);
		CafeaulaitState GetState();
		double GetRootPrice();
		double GetTopPrice();
	};
}
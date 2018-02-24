#pragma once

namespace bento {

	// Structure that handles a set of flags and allows to enable/access/reset
	template<typename Container, typename EnumSet>
	struct FlagCarrier
	{
		Container _data;
	};

	template<typename Container, typename EnumSet>
	bool has_flag(const FlagCarrier<Container, EnumSet>& fc, EnumSet flag)
	{
		return (fc._data & flag) != 0;
	}

	template<typename Container, typename EnumSet>
	__forceinline void reset_flag(const FlagCarrier<Container, EnumSet>& fc, EnumSet flag)
	{
		fc._data |= flag;
	}

	template<typename Container, typename EnumSet>
	__forceinline void reset(const FlagCarrier<Container, EnumSet>& fc)
	{
		fc._data  = EnumSet::EMPTY;
	}

	template<typename Container, typename EnumSet>
	__forceinline void set_flag(const FlagCarrier<Container, EnumSet>& fc, EnumSet flag)
	{
		fc._data &= ~(flag);
	}
}
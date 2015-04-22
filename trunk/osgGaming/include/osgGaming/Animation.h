#pragma once

#include <math.h>

#include <osg/Referenced>

namespace osgGaming
{
	typedef enum _animationEase
	{
		CIRCLE_IN,
		CIRCLE_OUT,
		CIRCLE_IN_OUT,
		LINEAR
	} AnimationEase;

	template <class T>
	class Animation : public osg::Referenced
	{
	public:
		Animation()
			: osg::Referenced(),
			  _animationStarted(false)
		{

		}

		Animation(T value, double duration, AnimationEase ease)
			: osg::Referenced(),
			  _value(value),
			  _fromValue(value),
			  _duration(duration),
			  _ease(ease),
			  _animationStarted(false)
		{

		}

		void beginAnimation(T fromValue, T toValue, double timeBegin)
		{
			_fromValue = fromValue;
			_value = fromValue;
			_toValue = toValue;
			_timeBegin = timeBegin;

			_animationStarted = true;
		}

		void beginAnimation(T toValue, double timeBegin)
		{
			beginAnimation(_value, toValue, timeBegin);
		}

		bool running()
		{
			return _animationStarted;
		}

		T getValue(double time)
		{
			if (_animationStarted)
			{
				double elapsed = (time - _timeBegin) / _duration;
				if (elapsed < 1.0)
				{
					switch (_ease)
					{
					case CIRCLE_OUT:
						elapsed = sqrt(1.0 - (elapsed * elapsed - 2 * elapsed + 1));
						break;
					case CIRCLE_IN:
						elapsed = 1.0 - sqrt(1.0 - elapsed * elapsed);
						break;
					case CIRCLE_IN_OUT:
						if (elapsed < 0.5)
						{
							elapsed = (1.0 - sqrt(1.0 - (4.0 * elapsed * elapsed))) * 0.5;
						}
						else
						{
							elapsed = sqrt(8.0 * elapsed - 4 * elapsed * elapsed - 3.0) * 0.5 + 0.5;
						}
						break;
					}

					_value = _fromValue + (_toValue - _fromValue) * elapsed;
				}
				else
				{
					_value = _toValue;
					_animationStarted = false;
				}
			}

			return _value;
		}

		void setDuration(double duration)
		{
			_duration = duration;
		}

		void setFromValue(T fromValue)
		{
			_fromValue = fromValue;
			_value = fromValue;
		}

		void setEase(AnimationEase ease)
		{
			_ease = ease;
		}

	private:
		T _value;
		T _fromValue;
		T _toValue;

		double _duration;
		double _timeBegin;

		bool _animationStarted;

		AnimationEase _ease;
	};

}
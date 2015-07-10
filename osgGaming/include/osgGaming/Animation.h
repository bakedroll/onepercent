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
		LINEAR,
		SMOOTH,
		SMOOTHER
	} AnimationEase;

	template <typename T>
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

		void setValue(T value)
		{
			_value = value;
			_animationStarted = false;
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

					case SMOOTH:

						elapsed = elapsed * elapsed * (3.0 - 2.0 * elapsed);
						break;

					case SMOOTHER:

						elapsed = elapsed * elapsed * elapsed * (elapsed * (elapsed * 6.0 - 15.0) + 10.0);
						break;

					}

					_value = relocate(_fromValue, _toValue, elapsed);
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

	protected:
		virtual T relocate(const T& from, const T& to, float elapsed)
		{
			return from + (to - from) * elapsed;
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

	template <typename T>
	class RepeatedSpaceAnimation : public Animation <T>
	{
	public:
		RepeatedSpaceAnimation(const T min, const T max)
			: Animation<T>(),
			  _min(min),
			  _max(max)
		{

		}

		RepeatedSpaceAnimation(const T min, const T max, T value, double duration, AnimationEase ease)
			: Animation<T>(value, duration, ease),
			  _min(min),
			  _max(max)
		{

		}

	protected:
		virtual T relocate(const T& from, const T& to, float elapsed)
		{
			T result;

			for (int i = 0; i < T::num_components; i++)
			{
				float l = _max._v[i] - _min._v[i];

				float fromv = from._v[i];
				float tov = to._v[i];
				while (fromv < _min._v[i])
					fromv += l;
				while (tov < _min._v[i])
					tov += l;
				while (fromv > _max._v[i])
					fromv -= l;
				while (tov > _max._v[i])
					tov -= l;

				float ab = tov - fromv;
				float ababs = std::abs(ab);

				if (ababs <= l / 2.0f)
				{
					result._v[i] = fromv + ab * elapsed;
				}
				else
				{
					result._v[i] = fromv - ab * elapsed * ((l - ababs) / ababs);

					if (result._v[i] < _min._v[i])
					{
						result._v[i] += l;
					}
					else if (result._v[i] > _max._v[i])
					{
						result._v[i] -= l;
					}
				}
			}

			return result;
		}

	private:
		T _min;
		T _max;
	};

}
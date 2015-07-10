#pragma once

#include <math.h>

#include <osg/Referenced>
#include <osg/Vec2f>
#include <osg/Vec3f>
#include <osg/Vec4f>

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
		float relocateValue(float from, float to, const float min, const float max, const float elapsed)
		{
			float result;
			float l = max - min;

			while (from < min)
				from += l;
			while (to < min)
				to += l;
			while (from > max)
				from -= l;
			while (to > max)
				to -= l;

			float ab = to - from;
			float ababs = std::abs(ab);

			if (ababs <= l / 2.0f)
			{
				result = from + ab * elapsed;
			}
			else
			{
				result = from - ab * elapsed * ((l - ababs) / ababs);

				if (result < max)
				{
					result += l;
				}
				else if (result > max)
				{
					result -= l;
				}
			}

			return result;
		}

		T _min;
		T _max;
	};

	class RepeatedScalarfAnimation : public RepeatedSpaceAnimation <float>
	{
	public:
		RepeatedScalarfAnimation(const float min, const float max)
			: RepeatedSpaceAnimation <float>(min, max)
		{

		}

		RepeatedScalarfAnimation(const float min, const float max, float value, double duration, AnimationEase ease)
			: RepeatedSpaceAnimation <float>(min, max, value, duration, ease)
		{

		}

	protected:
		virtual float relocate(const float& from, const float& to, float elapsed) override
		{
			return relocateValue(from, to, _min, _max, elapsed);
		}
	};

	template <typename T>
	class RepeatedVectorfAnimation : public RepeatedSpaceAnimation <T>
	{
	public:
		RepeatedVectorfAnimation(const T min, const T max)
			: RepeatedSpaceAnimation <T>(min, max)
		{

		}

		RepeatedVectorfAnimation(const T min, const T max, T value, double duration, AnimationEase ease)
			: RepeatedSpaceAnimation <T>(min, max, value, duration, ease)
		{

		}

	protected:
		virtual T relocate(const T& from, const T& to, float elapsed) override
		{
			T result;

			for (int i = 0; i < T::num_components; i++)
			{
				result._v[i] = relocateValue(from._v[i], to._v[i], _min._v[i], _max._v[i], elapsed);
			}

			return result;
		}
	};

	typedef RepeatedVectorfAnimation<osg::Vec2f> RepeatedVec2fAnimation;
	typedef RepeatedVectorfAnimation<osg::Vec3f> RepeatedVec3fAnimation;
	typedef RepeatedVectorfAnimation<osg::Vec4f> RepeatedVec4fAnimation;
}
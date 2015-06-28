#pragma once

#include <functional>

#include <osg/Referenced>

namespace osgGaming
{
	class Timer : public osg::Referenced
	{
	public:
		typedef std::function<void()> Callback;

		Timer();
		Timer(Callback callback, double duration = 1.0, bool singleShot = true);

		bool update(double time);

		void start();
		void stop();
		void setDuration(double duration);
		void setSingleShot(bool singleShot);
		void setCallback(Callback callback);

		bool running();

	private:
		bool _running;
		bool _singleShot;
		bool _started;
		bool _stopped;

		double _duration;
		double _timeStarted;

		Callback _callback;

	};
}
#include <gtest/gtest.h>

#include <osgGaming/Observable.h>

TEST(ObservableTest, Observers)
{
  osgGaming::Observable<int> observedNumber(5);

  bool obs1Notified = false;
  osgGaming::Observer<int>::Ptr obs1 = observedNumber.connect([&obs1Notified](int value)
  {
    obs1Notified = true;
  });

  bool obs2Notified = false;
  osgGaming::Observer<int>::Ptr obs2 = observedNumber.connect([&obs2Notified](int value)
  {
    obs2Notified = true;
  });

  bool obs3Notified = false;
  osgGaming::Observer<int>::Ptr obs3 = observedNumber.connectAndNotify([&obs3Notified](int value)
  {
    obs3Notified = true;
  });

  EXPECT_EQ(observedNumber.get(), 5);
  EXPECT_FALSE(obs1Notified);
  EXPECT_FALSE(obs2Notified);
  EXPECT_TRUE(obs3Notified);

  obs3Notified = false;

  observedNumber.set(3);

  EXPECT_EQ(observedNumber.get(), 3);
  EXPECT_TRUE(obs1Notified);
  EXPECT_TRUE(obs2Notified);
  EXPECT_TRUE(obs3Notified);

  obs1Notified = false;
  obs2Notified = false;
  obs3Notified = false;

  obs2.release();

  observedNumber.set(10);

  EXPECT_EQ(observedNumber.get(), 10);
  EXPECT_TRUE(obs1Notified);
  EXPECT_FALSE(obs2Notified);
  EXPECT_TRUE(obs3Notified);
}
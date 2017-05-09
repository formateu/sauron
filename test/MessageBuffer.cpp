#include <thread>
#include <chrono>
#include "../Message.h"
#include "../MessageBuffer.h"


BOOST_AUTO_TEST_CASE(msg_buffer_nospinlock) {

  MessageBuffer msgBuffer;

  // trying to read while buffer is empty
  std::thread readerThread([&msgBuffer]() {
      msgBuffer.pop();
    });

  std::thread writerThread([&msgBuffer]() {
      // waiting a moment to ensure reader thread executes .pop()
      std::this_thread::sleep_for(std::chrono::seconds(1));
      msgBuffer.push({"127.0.0.1", Message()});
    });

  readerThread.join();
  writerThread.join();

  // spinlock haven't occured
  BOOST_TEST(true);
}

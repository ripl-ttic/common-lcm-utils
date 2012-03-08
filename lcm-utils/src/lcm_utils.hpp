#ifndef __LCM_UTILS_HPP_
#define __LCM_UTILS_HPP_

#include <lcm/lcm-cpp.hpp>
#include <iostream>

namespace lcm_utils {

template<typename LcmType>
typename std::vector<LcmType> loadMsgsFromLog(const std::string& logfileName, const std::string & channel)
{
  typename std::vector<LcmType> ret;
  // Open the log file.
  lcm::LogFile log(logfileName.c_str(), "r");
  if (!log.good()) {
    perror("LogFile");
    std::cerr << "couldn't open log file: " << logfileName << std::endl;
    return ret;
  }

  while (1) {
    // Read a log event.
    const lcm::LogEvent *event = log.readNextEvent();
    if (!event)
      break;

    // Only process messages on the desired channel.
    if (event->channel != channel)
      continue;

    // Try to decode the message.
    LcmType msg;
    if (msg.decode(event->data, 0, event->datalen) != event->datalen) {
      std::cerr << "WARNING: Could not decode msg on channel: " << channel << " incorrect template arg?\n";
      continue;
    }

    ret.push_back(msg);
  }
  // Log file is closed automatically when the log variable goes out of scope.

  if (ret.empty()) {
    std::cerr << "WARNING: No Messages found/decoded on channel: " << channel << std::endl;
  }

  return ret;
}

template<typename LcmType>
void writeMsgToFile(const std::string& logfileName, const LcmType & msg)
{
  lcm::LogFile log(logfileName.c_str(), "w");
  if (!log.good()) {
    perror("LogFile");
    std::cerr << "couldn't open log file: " << logfileName << std::endl;
    return;
  }
  lcm::LogEvent event;
  event.channel = "SINGLEMSGLOG";
  event.timestamp = 0;
  event.eventnum = 0;
  event.datalen = msg.getEncodedSize();
  event.data = malloc(event.datalen); //is freed by event's destructor
  msg.encode(event.data, 0, event.datalen);
  log.writeEvent(&event);
}

template<typename LcmType>
LcmType readMsgFromFile(const std::string& logfileName)
{
  LcmType msg;
  lcm::LogFile log(logfileName.c_str(), "r");
  if (!log.good()) {
    perror("LogFile");
    std::cerr << "ERROR: couldn't open log file: " << logfileName << std::endl;
    return msg;
  }
  const lcm::LogEvent *event = log.readNextEvent();
  if (event ==NULL) {
    std::cerr << "ERROR: log is invalid.\n";
    return msg;
  }
  else if (event->channel != "SINGLEMSGLOG") {
    std::cerr << "ERROR: channel of "<< event->channel << " doesn't match SINGLEMSGLOG\n";
    return msg;
  }
  // Try to decode the message.
  if (msg.decode(event->data, 0, event->datalen) != event->datalen) {
    std::cerr << "ERRPR: Could not decode msg... incorrect template arg?\n";
  }
  return msg;
}

}

#endif

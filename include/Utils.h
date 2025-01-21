#ifndef UTILS_H
#define UTILS_H
#include <iostream>
#include <sstream>
#include <iostream>
#include <chrono>
#include <stdexcept>
#include <string>
#include <filesystem> // For filesystem operations
#include <fstream>    // For file I/O
#include <vector>
#include <tuple>
#include <rapidjson/document.h>
#include <thread>
#include <future>
#include <iomanip> // For std::setprecision
#include <sstream> // for stringstream
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <rapidjson/prettywriter.h> 
#include "System.h"
using namespace std;

string jsonToString(const rapidjson::Document& doc);
void showNetworkMenu();
void showTradeMenu();
void testOrderPlacement(int numCalls, string token, System& system);
#endif
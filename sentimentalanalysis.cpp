#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <regex>
#include <map>
#include <cmath>
#include <sstream>
#include "curl/curl.h"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

const string CONSUMER_KEY = "Hi0hmJvJcUUxmtl0XL5tp0mjG";
const string CONSUMER_SECRET = "ce2U4Czd05cKJpURhhCwVqACoW7u27wfcYvbE4fESPQdt0NMq7";
const string ACCESS_TOKEN = "707077794455429120-WB1PlMIomZOXkS4kB6gz0itt76ETlnc";
const string ACCESS_TOKEN_SECRET = "XY0JoWthhpg6bWZ6zsgiQHKEJkx0TtjEaZuNjUiyV1umb";

const string TWITTER_API_BASE_URL = "https://api.twitter.com/1.1/search/tweets.json";

string twitterQuery = "";

size_t CurlWrite_CallbackFunc_StdString(void* contents, size_t size, size_t nmemb, string* s)
{
    size_t newLength = size * nmemb;
    try
    {
        s->append((char*)contents, newLength);
    }
    catch (const bad_alloc& e)
    {
        return 0;
    }
    return newLength;
}

string PerformHttpGetRequest(const string& url)
{
    CURL* curl;
    CURLcode res;

    curl = curl_easy_init();
    string response;

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrite_CallbackFunc_StdString);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);

        curl_easy_cleanup(curl);

        if (res != CURLE_OK)
        {
            cerr << "Curl error: " << curl_easy_strerror(res) << endl;
        }
    }
    return response;
}

string CleanTweetText(const string& tweetText)
{
    string cleanedText = regex_replace(tweetText, regex("@[A-Za-z0-9_]+|https?://[^\\s]+|[^A-Za-z0-9]+"), " ");
    return cleanedText;
}

string GetTweetSentiment(const string& tweetText)
{
    return "positive";
}

void GetTweetsAndAnalyzeSentiment(const string& query, int count = 10)
{
    stringstream urlBuilder;
    urlBuilder << TWITTER_API_BASE_URL << "?q=" << query << "&count=" << count;

    string url = urlBuilder.str();

    string jsonResponse = PerformHttpGetRequest(url);

    json responseJson = json::parse(jsonResponse);

    vector<json> tweets = responseJson["statuses"];

    map<string, int> sentimentCounts;
    int totalTweets = tweets.size();

    for (const json& tweet : tweets)
    {
        string tweetText = tweet["text"];
        string cleanedText = CleanTweetText(tweetText);
        string sentiment = GetTweetSentiment(cleanedText);

        sentimentCounts[sentiment]++;
    }

    int positiveCount = sentimentCounts["positive"];
    int negativeCount = sentimentCounts["negative"];
    int neutralCount = sentimentCounts["neutral"];

    double positivePercentage = (double)positiveCount / totalTweets * 100.0;
    double negativePercentage = (double)negativeCount / totalTweets * 100.0;
    double neutralPercentage = (double)neutralCount / totalTweets * 100.0;

    cout << "Positive tweets percentage: " << positivePercentage << " %" << endl;
    cout << "Negative tweets percentage: " << negativePercentage << " %" << endl;
    cout << "Neutral tweets percentage: " << neutralPercentage << " %" << endl;
}

int main()
{
    cout << "Enter The Keyword:";
    cin >> twitterQuery;

    GetTweetsAndAnalyzeSentiment(twitterQuery, 500);

    return 0;
}

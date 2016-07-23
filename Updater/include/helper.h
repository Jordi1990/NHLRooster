#include "boost/date_time/posix_time/posix_time.hpp"
#include <cstdio>

using namespace boost::posix_time;

/*
========================
writer
Callback function for CURL which writes data to our buffer
========================
*/
static int writer(char *data, size_t size, size_t nmemb, std::string *writerData)
{
    if (writerData == NULL)
        return 0;

    writerData->append(data, size*nmemb);

    return size * nmemb;
}

/*
========================
replaceAll
Replaces all occurences with given string
========================
*/
std::string replaceAll(std::string str, const std::string& from, const std::string& to)
{
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

/*
========================
getStringBetween
Returns the string which is between the other 2 strings by using a regular expression
========================
*/
std::string getStringBetween(const std::string& before, const std::string& after, const std::string &line, const std::string regex_between = "(.*)")
{
    // use streams to concat values
    std::string result;
    std::stringstream ss;
    ss << before << regex_between << after;
    std::string regex_str = ss.str();
    const std::regex e(regex_str);
    std::sregex_iterator next(line.begin(), line.end(), e);
    std::smatch match = *next;
    result = match.str();
    result = replaceAll(result, before, "");
    result = replaceAll(result, after, "");
    return result;
}

/*
========================
ltrim
Trims whitespace\tabs from start of given string
========================
*/
inline std::string& ltrim(std::string& string, const char* t = " \t\n\r\f\v")
{
	string.erase(0, string.find_first_not_of(t));
	return string;
}

/*
========================
rtrim
Trims whitespace\tabs from end of given string
========================
*/
inline std::string& rtrim(std::string& string, const char* t = " \t\n\r\f\v")
{
	string.erase(string.find_last_not_of(t) + 1);
	return string;
}

/*
========================
trim
Trims whitespace\tabs from start and end of given string
========================
*/
inline std::string& trim(std::string& s, const char* t = " \t\n\r\f\v")
{
    return ltrim(rtrim(s, t), t);
}

/*
========================
getCurrentWeek
Returns the current week number(1-52)
========================
*/
int getCurrentWeekNumber()
{
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[4];
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, 4, "%W", timeinfo);   // '%W' = week number of the year, eg 1/1/09 == 1
	return atoi(buffer) + 1;     // convert char to int
}

/*
========================
getWeekNrFromDate
Returns the week number(1-52) from given date according to ISO 8601
========================
*/
int getWeekNrFromDate(const std::string& date)
{
	//2015-04-29T15:15:00
	int year, month, day, hour, minute, seconds;
	sscanf(date.c_str(), "%d-%d-%dT%d:%d:%d", &year, &month, &day, &hour, &minute, &seconds);
    struct tm tm;
	tm.tm_hour = hour;
	tm.tm_min = minute;
	tm.tm_sec = seconds;
	tm.tm_mday = day;
	tm.tm_mon = month - 1;
	tm.tm_year = year - 1900;
	mktime(&tm);
    char timebuf[64];
	strftime(timebuf, sizeof(timebuf), "%W", &tm);
    return atoi(timebuf) + 1;
}

/*
========================
getTimeStampFromDate
Returns timestamp generated from given date
Date has to be in format YYYY-MM-DDTHH:MM:SS
========================
*/
unsigned long getTimeStampFromDate(const std::string& date)
{
	int year, month, day, hour, minute, seconds;
	sscanf(date.c_str(), "%d-%d-%dT%d:%d:%d", &year, &month, &day, &hour, &minute, &seconds);
	struct tm tm;
	tm.tm_hour = hour;
	tm.tm_min = minute;
	tm.tm_sec = seconds;
	tm.tm_mday = day;
	tm.tm_mon = month - 1;
	tm.tm_year = year - 1900;
    return (long)mktime(&tm);
}

/*
========================
getTimeStampFromDateAsString
Returns timestamp generated from given date as string
Date has to be in format YYYY-MM-DDTHH:MM:SS
========================
*/
char *getTimeStampFromDateAsString(const std::string& date)
{
	unsigned long timestamp = getTimeStampFromDate(date) * 1000;
	char timestampStr[64];
	int n = sprintf(timestampStr, "%lu", timestamp);
	timestampStr[n] = '\0';
	return timestampStr;
}

/*
========================
currentDateTime
Returns current date
Format: YYY-MM-DD HH:MM:SS
========================
*/
const std::string currentDateTime()
{
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    return buf;
}

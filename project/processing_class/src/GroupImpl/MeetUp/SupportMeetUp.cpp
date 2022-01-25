#include <sstream>

#include <MeetUpImpl.hpp>


duration_t StrToDuration(const std::string &time) {
    std::stringstream stream(time);

    int h, m = 0;

    stream >> h;
    stream.ignore(1);
    stream >> m;

    int total = h * 60 + m;

    duration_t dur;
    dur.hour = total / 60;
    dur.min = total % 60;

    return dur;
}

int operator-(const duration_t& lhs, const duration_t& rhs) {
    return lhs.total - rhs.total;
};

std::string DurationToStr(const duration_t& time) {
    std::string time_s = std::to_string(time.hour) + ":" + std::to_string(time.min);

    return time_s;
}

duration_t CharToDuration(const unsigned char& time_interval) {
    int total = int(time_interval) * 15;
    duration_t duration;
    duration.hour = total / 60;
    duration.min = total % 60;
    return duration;
}

unsigned char DurarationToChar(const duration_t& duraton) {
    int total = duraton.hour * 60 * duraton.min;
    return (unsigned char)(total / 15 + 1);
}

unsigned char StrToChar(const std::string& time) {
    return (DurarationToChar(StrToDuration(time)));
};

std::string CharToStr(const unsigned char& time) {
    return (DurationToStr(CharToDuration(time)));
}


//  support class Day


Day::Day() {
    storage = new unsigned char [NUMBER_INTERVAL];
}

Day::~Day() {
    delete [] storage;
}

unsigned char* Day::GetStorage() const {
    return storage;
}


void Day::InsertEvent(std::string& begin_time, std::string& end_time) {
    unsigned char begin = StrToChar(begin_time);
    unsigned char end = StrToChar(end_time);

    while (begin < end) {
        storage[begin / BITS] |= ((unsigned char)1 << (begin % BITS));
        begin ++;
    }
}

void Day::EraseEvent(std::string& begin_time, std::string& end_time) {
    unsigned char begin = StrToChar(begin_time);
    unsigned char end = StrToChar(end_time);

    while (begin < end) {
        storage[begin / BITS] &= ~((unsigned char)1 << (begin % BITS));
        begin++;
    }
}

void Day::UnionDays(Day& added_day) {
    for (unsigned char i = 0; i < NUMBER_INTERVAL; i ++) {
        storage[i] |= added_day.GetStorage()[i];
    };
}

void Day::InvertDay(Day& busy_day) {
    for (unsigned char i = 0; i < NUMBER_INTERVAL; i ++) {
        storage[i] = ~busy_day.GetStorage()[i];
    };
}

bool Day::IsFree(unsigned char time_interval) {
    return (storage[time_interval / BITS] >> (time_interval % BITS)) & 1; 
}


bool IsMeetUp (duration_t duration1, duration_t duration2) {
    if (duration2 - duration1 <= 15) {
        return true;
    } else {
        return false;
    }
}

Day CreateFreeDay(std::vector<std::set<event_t>> members_events) {
    Day busy_day;

    for (auto user_events: members_events) {

        Day user_day;

        for (auto event: user_events) {
            user_day.InsertEvent(event.time_begin, event.time_end);
        }

        busy_day.UnionDays(user_day);
    }

    Day free_day;

    free_day.InvertDay(busy_day);

    return free_day;
}


std::set<std::string> Day::GetSetOfFreeTime() {
    std::set<std::string> all_free_times;

    for (unsigned char i = 0; i < NUMBER_INTERVAL; i++) {
        if ((storage[i / BITS] >> (i % BITS)) & 1) {
            std::string time = CharToStr(i);
            all_free_times.insert(time);
        }
    }
    return all_free_times;
}


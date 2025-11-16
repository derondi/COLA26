#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include<cstdint>
#include<string>
#include<iostream>

typedef uint64_t sim_time_type;
typedef uint16_t stream_id_type;
typedef sim_time_type data_timestamp_type;

#define INVALID_TIME 18446744073709551615ULL
#define T0 0
#define INVALID_TIME_STAMP 18446744073709551615ULL
#define MAXIMUM_TIME 18446744073709551615ULL
#define ONE_SECOND 1000000000
typedef std::string sim_object_id_type;

#define CurrentTimeStamp Simulator->Time()
#define PRINT_ERROR(MSG) {\
							std::cerr << "ERROR:" ;\
							std::cerr << MSG << std::endl; \
							std::cin.get();\
							exit(1);\
						 }
#define PRINT_MESSAGE(M) std::cout << M << std::endl;
#define DEBUG(M) //std::cout<<M<<std::endl;
#define DEBUG2(M) //std::cout<<M<<std::endl;
#define SIM_TIME_TO_MICROSECONDS_COEFF 1000
#define SIM_TIME_TO_SECONDS_COEFF 1000000000


// For Debug perpose
static void get_timestamp_thread_safe(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm local_time_struct;

    if (localtime_r(&now, &local_time_struct) == NULL) {
        perror("localtime_r");
        buffer[0] = '\0';
        return;
    }

    if (strftime(buffer, size, "%Y-%m-%d %H:%M:%S", &local_time_struct) == 0) {
        buffer[0] = '\0';
    }
}

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

extern bool is_preconditioning;

// #define MQSIM_DEBUG

#ifdef MQSIM_DEBUG
#define LOG(fmt, ...)                                                                                                                                                                             \
    do {                                                                                                                                                                                          \
      if (!is_preconditioning)                                                                                                                                                                        \
      {                                                                                                                                                                                           \
        char current_timestamp[20];                                                                                                                                                                       \
        get_timestamp_thread_safe(current_timestamp, sizeof(current_timestamp));                                                                                                                                  \
        fprintf(stderr, "%s [MQSim] " GREEN "%s" RESET ":%d" BLUE " %s" RESET " Log: " RED fmt "\n" RESET, current_timestamp, __FILE__, __LINE__, __PRETTY_FUNCTION__ /* __FUNCTION__ */, ##__VA_ARGS__); \
        fflush(stderr);                                                                                                                                                                           \
      }                                                                                                                                                                                           \
    } while(0)
#else

#define LOG(...)

#endif

#endif // !DEFINITIONS_H

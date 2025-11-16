#ifndef ASCII_TRACE_DEFINITION_H
#define ASCII_TRACE_DEFINITION_H

enum class Trace_Time_Unit { PICOSECOND, NANOSECOND, MICROSECOND};//The unit of arrival times in the input file
#define PicoSecondCoeff  1000000000000	//the coefficient to convert picoseconds to second
#define NanoSecondCoeff  1000000000	//the coefficient to convert nanoseconds to second
#define MicroSecondCoeff  1000000	//the coefficient to convert microseconds to second

#define ALIYUN_TRACE

#ifdef ALIYUN_TRACE
#define ASCIITraceDeviceColumn 0
#define ASCIITraceTypeColumn 1
#define ASCIITraceAddressColumn 2
#define ASCIITraceSizeColumn 3
#define ASCIITraceTimeColumn 4
#define ASCIITraceWriteCode "W,"
#define ASCIITraceReadCode "R,"
#define ASCIILineDelimiter ','
#define ASCIIItemsPerLine 5

#else

#define ASCIITraceTimeColumn 0
#define ASCIITraceDeviceColumn 1
#define ASCIITraceAddressColumn 2
#define ASCIITraceSizeColumn 3
#define ASCIITraceTypeColumn 4
#define ASCIITraceWriteCode "0"
#define ASCIITraceReadCode "1"
#define ASCIITraceWriteCodeInteger 0
#define ASCIITraceReadCodeInteger 1
#define ASCIILineDelimiter ' '
#define ASCIIItemsPerLine 5

#endif

#endif // !ASCII_TRACE_DEFINITION_H

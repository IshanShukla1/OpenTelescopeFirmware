#ifndef CORE_RESULT_H
#define CORE_RESULT_H

#include <utility>

namespace core {

enum class StatusCode {
    OK = 0,
    INVALID_ARGUMENT,
    INVALID_STATE,
    CONFIG_ERROR,
    NOT_INITIALIZED,
    SAFETY_INHIBITED,
    FAULT_ACTIVE,
    HARDWARE_ERROR,
    BUFFER_FULL,
    UNKNOWN_COMMAND,
    TIMEOUT
};

template <typename T>
class Result {
public:
    static Result Success(T value) {
        return Result(std::move(value), StatusCode::OK);
    }

    static Result Failure(StatusCode code) {
        return Result(T{}, code);
    }

    bool is_ok() const { return code_ == StatusCode::OK; }
    bool is_error() const { return code_ != StatusCode::OK; }

    const T& value() const { return value_; }
    T& value() { return value_; }
    StatusCode status() const { return code_; }

private:
    Result(T val, StatusCode code) : value_(std::move(val)), code_(code) {}

    T value_;
    StatusCode code_;
};

template <>
class Result<void> {
public:
    static Result Success() {
        return Result(StatusCode::OK);
    }

    static Result Failure(StatusCode code) {
        return Result(code);
    }

    bool is_ok() const { return code_ == StatusCode::OK; }
    bool is_error() const { return code_ != StatusCode::OK; }

    StatusCode status() const { return code_; }

private:
    explicit Result(StatusCode code) : code_(code) {}

    StatusCode code_;
};

} // namespace core

#endif // CORE_RESULT_H

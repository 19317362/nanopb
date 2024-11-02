#include <stdio.h>
#include <string.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include "user.pb.h"

/* 回调函数，用于编码字符串 */
static bool encode_string_callback(pb_ostream_t *stream, const pb_field_t *field, void * const *arg) {
    const char *str = (const char*)(*arg);
    if (!pb_encode_tag_for_field(stream, field)) {
        return false;
    }
    return pb_encode_string(stream, (const uint8_t*)str, strlen(str));
}
// 回调函数，用于处理解码后的字符串
static bool decode_string_callback(pb_istream_t *stream, const pb_field_t *field, void **arg) {
    char *buffer = (char *)*arg;
    if (stream->bytes_left > 100) {
        // 字符串太长，无法处理
        return false;
    }
    if (!pb_read(stream, (pb_byte_t *)buffer, stream->bytes_left)) {
        return false;
    }
    buffer[stream->bytes_left] = '\0'; // 确保字符串以 null 结尾
    return true;
}

int main() {
    /* 创建一个 testpb_User 结构体并初始化 */
    testpb_User user = testpb_User_init_default;
    user.id = 12345;
    //user.name.funcs.encode = &encode_string_callback;
    //user.name.arg = "John";
    strcpy(user.name, "John");
    //user.email.funcs.encode = &encode_string_callback;
    //user.email.arg = "john@qq.com";
    strcpy(user.email, "john@qq.com");
    /* 序列化 */
    uint8_t buffer[128];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    if (!pb_encode(&stream, testpb_User_fields, &user)) {
        printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
        return 1;
    }

    size_t message_length = stream.bytes_written;
    printf("Encoded message length: %zu\n", message_length);
#if 1
    /* 反序列化 */
    testpb_User decoded_user = testpb_User_init_zero;
    pb_istream_t istream = pb_istream_from_buffer(buffer, message_length);
    //decoded_user.name.funcs.decode = &decode_string_callback;
    //decoded_user.email.funcs.decode = &decode_string_callback;

    if (!pb_decode(&istream, testpb_User_fields, &decoded_user)) {
        printf("Decoding failed: %s\n", PB_GET_ERROR(&istream));
        return 1;
    }

    printf("Decoded user ID: %d\n", decoded_user.id);
    printf("Decoded user name: %s\n", decoded_user.name);
    printf("Decoded user email: %s\n", decoded_user.email);
#endif
    return 0;
}
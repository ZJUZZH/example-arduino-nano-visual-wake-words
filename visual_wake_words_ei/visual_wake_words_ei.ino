/* Includes ---------------------------------------------------------------- */
#include <visual_wake_words_-_tinymlperf_inference.h>
#include <TinyMLShield.h>

// raw frame buffer from the camera
#define FRAME_BUFFER_COLS           176
#define FRAME_BUFFER_ROWS           144

/**
 * @brief      Convert monochrome data to rgb values
 *
 * @param[in]  mono_data  The mono data
 * @param      r          red pixel value
 * @param      g          green pixel value
 * @param      b          blue pixel value
 */
static inline void mono_to_rgb(uint8_t mono_data, uint8_t *r, uint8_t *g, uint8_t *b) {
    uint8_t v = mono_data;
    *r = *g = *b = v;
}

// Get an image from the camera module
int get_image(size_t offset, size_t length, float *out_ptr) {
    int8_t image_data[FRAME_BUFFER_COLS * FRAME_BUFFER_ROWS];
    size_t bytes_left = length;
    size_t out_ptr_ix = 0;

    byte data[FRAME_BUFFER_COLS * FRAME_BUFFER_ROWS]; // Receiving QCIF grayscale from camera = 176 * 144 * 1

    // Read camera data
    Camera.readFrame(data);
  
    int min_x = (FRAME_BUFFER_COLS - EI_CLASSIFIER_INPUT_WIDTH) / 2;
    int min_y = (FRAME_BUFFER_ROWS - EI_CLASSIFIER_INPUT_HEIGHT) / 2;
    int index = 0;
  
    // Crop 96x96 image. This lowers FOV, ideally we would downsample but this is simpler. 
    for (int y = min_y; y < min_y + EI_CLASSIFIER_INPUT_HEIGHT; y++) {
        for (int x = min_x; x < min_x + EI_CLASSIFIER_INPUT_WIDTH; x++) {
            image_data[index++] = static_cast<int8_t>(data[(y * 176) + x] - 128); // convert TF input image to signed 8-bit
        }
    }

    // read byte for byte
    while (bytes_left != 0) {

        // grab the value and convert to r/g/b
        uint8_t pixel = image_data[offset];

        uint8_t r, g, b;
        mono_to_rgb(pixel, &r, &g, &b);

        // then convert to out_ptr format
        float pixel_f = (r << 16) + (g << 8) + b;
        out_ptr[out_ptr_ix] = pixel_f;

        // and go to the next pixel
        out_ptr_ix++;
        offset++;
        bytes_left--;
    }

    // and done!
    return 0;
}


/**
 * @brief      Arduino setup function
 */
void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);

    Serial.println("Edge Impulse Person Detection Inferencing Demo");

    if (!Camera.begin(QCIF, GRAYSCALE, 5, OV7675)) {
        ei_printf("Failed to initialize camera!");
    }
}

/**
 * @brief      Arduino main function
 */
void loop()
{
    ei_printf("Edge Impulse inferencing (Arduino)\n");

    ei_impulse_result_t result = { 0 };


    // Set up pointer to look after data, crop it and convert it to RGB888
    signal_t signal;
    signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
    signal.get_data = &get_image;

    // invoke the impulse
    EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false /* debug */);
    ei_printf("run_classifier returned: %d\n", res);

    if (res != 0) return;

    // print the predictions
    ei_printf("Predictions ");
    ei_printf("(DSP: %d ms., Classification: %d ms., Anomaly: %d ms.)",
        result.timing.dsp, result.timing.classification, result.timing.anomaly);
    ei_printf(": \n");
    ei_printf("[");
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
        ei_printf("%.5f", result.classification[ix].value);
#if EI_CLASSIFIER_HAS_ANOMALY == 1
        ei_printf(", ");
#else
        if (ix != EI_CLASSIFIER_LABEL_COUNT - 1) {
            ei_printf(", ");
        }
#endif
    }
#if EI_CLASSIFIER_HAS_ANOMALY == 1
    ei_printf("%.3f", result.anomaly);
#endif
    ei_printf("]\n");

    // human-readable predictions
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
        ei_printf("    %s: %.5f\n", result.classification[ix].label, result.classification[ix].value);
    }
#if EI_CLASSIFIER_HAS_ANOMALY == 1
    ei_printf("    anomaly score: %.3f\n", result.anomaly);
#endif

}

/**
 * @brief      Printf function uses vsnprintf and output using Arduino Serial
 *
 * @param[in]  format     Variable argument list
 */
void ei_printf(const char *format, ...) {
    static char print_buf[1024] = { 0 };

    va_list args;
    va_start(args, format);
    int r = vsnprintf(print_buf, sizeof(print_buf), format, args);
    va_end(args);

    if (r > 0) {
        Serial.write(print_buf);
    }
}

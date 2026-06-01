namespace hal {

class Gpio {
public:
    Gpio() = delete;
    ~Gpio() = delete;
    static void init(const uint8_t pin);
    static void setOutput(const uint8_t pin);
    static void setInput(const uint8_t pin);
    static void pullUp(const uint8_t pin);
    static bool read(const uint8_t pin);
    static void write(const uint8_t pin, const bool value);
    static void setExternalInterrupt(const uint8_t pin, bool rising_edge, bool enable, void(*callback)(uint32_t, uint32_t));
    static bool checkRisingEdge(const uint32_t event);
};

}
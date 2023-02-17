#ifndef buttons_hpp_
#define buttons_hpp_

// const IL9341 button setup
#define BUTTON_1 38
#define BUTTON_2 37
#define BUTTON_3 39

class Button
{
  private:
    bool prevpres;
    bool sequence;
    unsigned long lastchange;
    static unsigned long constexpr button_delay = 50;
    static unsigned long constexpr sequence_delay_time = 250;
    static unsigned long constexpr sequence_start_delay_time = 250;
  public:
    int port;
    Button(int p)
    {
      port = p;
    }
    bool check_it()
    {
      bool pressed = !digitalRead(port);
      auto time_now = millis();
      if (pressed == true)
      {
        // Normal click
        if (prevpres == false)
        {
          if (time_now - lastchange >= button_delay)
          {
            lastchange = time_now;
            prevpres = pressed;
            return true;
          }
        }
      }
      else
      {
        prevpres = false;
        sequence = false;
      }
      return false;
    }
};

#endif
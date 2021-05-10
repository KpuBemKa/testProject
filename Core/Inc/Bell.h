/* enum BeepState
{
  Off,
  On
};

class Bell
{
  private:
    BeepState bellState;
    uint32_t lastBeepTime;
    uint32_t beepStartTime;

  public:
    Bell()
    {

    }

    Bell(BeepState bellState, uint32_t lastBeepTime, uint32_t beepStartTime)
    {
      this->bellState = bellState;
      this->lastBeepTime = lastBeepTime;
      this->beepStartTime = beepStartTime;
    }

    BeepState getBellState() {
      return this->bellState;
    }

    void setBellState(BeepState bellState) {
      this->bellState = bellState;

      if(bellState == On)
      {
        HAL_GPIO_WritePin(Zumer1_GPIO_Port, Zumer1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(Zumer2_GPIO_Port, Zumer2_Pin, GPIO_PIN_RESET);
      }
      else if(bellState == Off)
      {
        HAL_GPIO_WritePin(Zumer1_GPIO_Port, Zumer1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(Zumer2_GPIO_Port, Zumer2_Pin, GPIO_PIN_SET);
      }
    }

    uint32_t getLastBeepTime() {
      return this->lastBeepTime;
    }

    void setLastBeepTime(uint32_t lastBeepTime) {
      this->lastBeepTime = lastBeepTime;
    }

    uint32_t getBeepStartTime() {
      return this->beepStartTime;
    }

    void setBeepStartTime(uint32_t beepStartTime) {
      this->beepStartTime = beepStartTime;
    }
}; */
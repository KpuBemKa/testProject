/* class Door
{
  private:
    State doorState;
    bool doorStateChanged;

  public:
    Door();

    Door(State doorState)
    {
      this->doorState = doorState;
    }

    State getDoorState()
    {
      return doorState;
    }

    void setDoorState(State doorState)
    {
      this->doorState = doorState;
      this->doorStateChanged = true;
    }

    bool isDoorStateChanged()
    {
      return this ->doorStateChanged;
    }

    void setDoorStateChanged(bool doorStateChanged)
    {
      this ->doorStateChanged = doorStateChanged;
    }
}; */
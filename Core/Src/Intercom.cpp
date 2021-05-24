#include "Intercom.h"

bool
    localInsideKeyRead = false,
    localOutsideKeyRead = false;

bool verifyCode(uint32_t code);

Mode::~Mode()
{
}

void Mode::setMode(Context *context)
{
    this->context = context;
}

Context::Context(IntercomMode intercomMode) : mode()
{
    switch (intercomMode)
    {
    case IntercomMode::NormalMode:
    {
        this->TransitionTo(new NormalMode);
        break;
    }
    case IntercomMode::ClosedMode:
    {
        this->TransitionTo(new ClosedMode);
        break;
    }
    case IntercomMode::OpenMode:
    {
        this->TransitionTo(new OpenMode);
        break;
    }
    case IntercomMode::CondOpenMode:
    {
        this->TransitionTo(new CondOpenMode);
        break;
    }

    default:
        this->TransitionTo(nullptr);
        break;
    }
}

Context::~Context()
{
    delete this->mode;
}

void Context::TransitionTo(Mode *mode)
{
    if (this->mode == nullptr)
    {
        delete this->mode;
    }

    this->mode = mode;
    this->mode->setMode(this);
}

void Context::KeyReadEvent()
{
    this->mode->KeyReadHandle();
}

void NormalMode::KeyReadHandle()
{
    if (wig_available() && verifyCode(getCode()))
    {
        UART_Printf("%s intercom was used. Key id: %d.\r\n", localInsideKeyRead ? "Inside" : "Outside", getCode());
        workMode = WorkMode::TempOpenMode;
    }
}

void ClosedMode::KeyReadHandle()
{
    if (wig_available())
    {
        UART_Printf("Acces denied, door is closed. Key id: %d.\r\n", getCode());

        previousWorkMode = workMode;
        workMode = WorkMode::DenyMode;
    }
}

void OpenMode::KeyReadHandle()
{
    if (wig_available())
    {
        UART_Printf("%s intercom was used. Key id: %d.\r\n", localInsideKeyRead ? "Inside" : "Outside", getCode());
        workMode = WorkMode::TempOpenMode;
    }
}

void CondOpenMode::KeyReadHandle()
{
    if (wig_available())
    {
        UART_Printf("%s intercom was used. Key id: %d.\r\n", localInsideKeyRead ? "Inside" : "Outside", getCode());
        workMode = WorkMode::TempOpenMode;

        UART_Printf("Current mode is Conditionally Open. Switching to Open.\r\n");
        this->context->TransitionTo(new OpenMode);
    }
}

/**
 * @brief Verify if code is allowed to enter/exit
 * @retval bool
 * @param code to verify it's validity
*/
bool verifyCode(uint32_t code)
{
    uint32_t codes[] =
        {12563593};

    for (unsigned int i = 0; i < sizeof(codes) / sizeof(codes[0]); i++)
    {
        if (code == codes[i])
            return true;
    }

    return false;
}

void intercom(bool insideKey, bool outsideKey)
{
    localInsideKeyRead = insideKey;
    localOutsideKeyRead = outsideKey;

    Context *context = new Context(intercomMode);
    context->KeyReadEvent();
    delete context;

    localInsideKeyRead = false;
    localOutsideKeyRead = false;
}
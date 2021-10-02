#include <stdafx.h>

#include <host/host.h>

using namespace Qwr::DotnetHost;

namespace
{

class InitQuit : public initquit
{
public:
    void on_init() override;
    void on_quit() override;
};

} // namespace

namespace
{

void InitQuit::on_init()
{
    Host::GetInstance()->Start();
}

void InitQuit::on_quit()
{
    Host::GetInstance()->Shutdown();
}

}

namespace
{

FB2K_SERVICE_FACTORY( InitQuit );

}

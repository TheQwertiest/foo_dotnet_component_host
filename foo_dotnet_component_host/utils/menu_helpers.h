#pragma once

namespace Qwr::DotnetHost
{

void ExecuteContextMenuCommand( const std::string& name, const metadb_handle_list& p_handles, uint32_t flags );
void ExecuteMainMenuCommand( const std::string& name );

} // namespace smp::utils

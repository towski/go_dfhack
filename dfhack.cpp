#define LINUX_BUILD
#include <iostream>
#include <stdint.h>
#include <algorithm>    // std::sort
#include "dfhack.h"
#include "DataDefs.h"
#include "Export.h"
#include "library/proto/Basic.pb.h"
#include "RemoteClient.h"
#include "RemoteTools.h"
#include "BasicApi.pb.h"
#include "MiscUtils.h"
#include "misc_trait_type.h"
#include "profession.h"
#include "job_type.h"
#include "job.h"
#include "job_item.h"
#include "unit_thought_type.h"
#include "unit_flags1.h"
#include "unit_flags2.h"
#include "modules/Materials.h"
#include "df/dfhack_material_category.h"

//#include "library/include/df/item.h"
dfproto::ListUnitsIn *in;
dfproto::ListUnitsOut *out;
dfproto::GetWorldInfoOut *world_info_out;
dfproto::ListEnumsOut *enums_out;

std::vector<dfproto::BasicUnitInfo*> dwarves;
std::vector<CDwarf> cdwarves;
std::vector<std::string> elems = std::vector<std::string>();
DFHack::RemoteFunction<dfproto::ListUnitsIn, dfproto::ListUnitsOut> list_units;
DFHack::RemoteFunction<dfproto::EmptyMessage, dfproto::GetWorldInfoOut> get_world_info;
DFHack::RemoteFunction<dfproto::SetUnitLaborsIn> set_unit_labors;
DFHack::color_ostream_wrapper * df_network_out = new DFHack::color_ostream_wrapper(std::cout);
DFHack::RemoteClient * network_client = new DFHack::RemoteClient(df_network_out);
std::string output;
std::string thought_string = "";

void Connect(void) {
    printf("hey maybe\n");
    network_client->connect();
    printf("hey maybe2\n");
    DFHack::RemoteFunction<dfproto::EmptyMessage, dfproto::ListEnumsOut> list_enums;
    list_units.bind(network_client, "ListUnits");
    get_world_info.bind(network_client, "GetWorldInfo");
    list_enums.bind(network_client, "ListEnums");
    set_unit_labors.bind(network_client, "SetUnitLabors");
    in = new dfproto::ListUnitsIn();
    out = new dfproto::ListUnitsOut();
    printf("hey maybe3\n");
    in->set_scan_all(true);
    in->set_race(465);
    in->set_alive(true);
    in->mutable_mask()->set_profession(true);
    in->mutable_mask()->set_labors(true);
    in->mutable_mask()->set_skills(true);
    in->mutable_mask()->set_misc_traits(true);
    //my_call(network_client->default_output(), dfproto::ListUnitsIn::default_instance, dfproto::ListUnitsOut::default_instance);
    world_info_out = new dfproto::GetWorldInfoOut();
    printf("hey maybe4\n");

    std::ostringstream stream;
    DFHack::color_ostream_wrapper *df_output = new DFHack::color_ostream_wrapper(stream);
    network_client->run_command((*df_output), "collect_reactions", std::vector<std::string>());

    std::string item;
    std::istringstream iss(stream.str());
    while (std::getline(iss, item, '\n')) {
        elems.push_back(item);
    }
    printf("hey maybe5\n");
    std::cout << std::to_string(elems.size()) + "\n";

    enums_out = new dfproto::ListEnumsOut();
    printf("hey maybe6\n");
    list_enums(new dfproto::EmptyMessage(), enums_out);

}


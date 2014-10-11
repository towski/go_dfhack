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

bool sort_dwarves (dfproto::BasicUnitInfo* dwarf1, dfproto::BasicUnitInfo* dwarf2 ){
    return dwarf1->profession() < dwarf2->profession(); 
}

bool sort_cdwarves (CDwarf dwarf1, CDwarf dwarf2 ){
    return dwarf1.profession < dwarf2.profession; 
}

void Connect(void) {
    network_client->connect();
    DFHack::RemoteFunction<dfproto::EmptyMessage, dfproto::ListEnumsOut> list_enums;
    list_units.bind(network_client, "ListUnits");
    get_world_info.bind(network_client, "GetWorldInfo");
    list_enums.bind(network_client, "ListEnums");
    set_unit_labors.bind(network_client, "SetUnitLabors");
    in = new dfproto::ListUnitsIn();
    out = new dfproto::ListUnitsOut();
    in->set_scan_all(true);
    in->set_race(465);
    in->set_alive(true);
    in->mutable_mask()->set_profession(true);
    in->mutable_mask()->set_labors(true);
    in->mutable_mask()->set_skills(true);
    in->mutable_mask()->set_misc_traits(true);
    //my_call(network_client->default_output(), dfproto::ListUnitsIn::default_instance, dfproto::ListUnitsOut::default_instance);
    world_info_out = new dfproto::GetWorldInfoOut();

    std::ostringstream stream;
    DFHack::color_ostream_wrapper *df_output = new DFHack::color_ostream_wrapper(stream);
    network_client->run_command((*df_output), "collect_reactions", std::vector<std::string>());

    std::string item;
    std::istringstream iss(stream.str());
    while (std::getline(iss, item, '\n')) {
        elems.push_back(item);
    }
    std::cout << std::to_string(elems.size()) + "\n";

    enums_out = new dfproto::ListEnumsOut();
    list_enums(new dfproto::EmptyMessage(), enums_out);
}

bool SetUnitLabors(Labor* labors[], int size){
    dfproto::SetUnitLaborsIn *in = new dfproto::SetUnitLaborsIn();
    std::cout << "setting size" << size << "\n";
    for(int x = 0; x < size; x++){
        dfproto::UnitLaborState* labor = in->add_change();
        std::cout << "add labor" << labors[x]->labor << "\n";
        std::cout << "add labor" << labors[x]->value << "\n";
        std::cout << "add labor" << labors[x]->unit_id << "\n";
        labor->set_unit_id(labors[x]->unit_id);
        labor->set_labor(labors[x]->labor);
        labor->set_value(labors[x]->value);
    }
    std::cout << "sending labors" << "\n";
    set_unit_labors(in);
    return true;
}

CDwarf* GetDwarf(int i){
    return &cdwarves.at(i);
}

void Update() {
    out = new dfproto::ListUnitsOut();
    int result = get_world_info(new dfproto::EmptyMessage(), world_info_out);
    if(result != 0){
        std::cout << " failed to get world!" << result << "\n";
        network_client->disconnect();
        exit(1);
    }
    list_units(in, out);
    dwarves.clear();
    dfproto::BasicUnitInfo* dwarf;
    int j = 0;
    for(int i = 0; i < out->value_size(); i++){
        dwarf = (dfproto::BasicUnitInfo*)&out->value(i);
        df::unit_flags1 flags1 = dwarf->flags1();
        df::unit_flags2 flags2 = dwarf->flags2();
        if (!flags1.bits.merchant && !flags1.bits.forest && !flags1.bits.diplomat && !flags2.bits.visitor){
            dwarves.push_back(dwarf);
            std::string name = dwarf->name().first_name() + " " + dwarf->name().last_name() + " \n" + dwarf->profession_name(); 
            name[0] = toupper(name[0]);
            char * cstr = new char [name.length()+1];
            strcpy (cstr, name.c_str());
            if(cdwarves.size() <= j){
                CDwarf cdwarf = { 
                    .Id = dwarf->unit_id(),
                    .name = cstr,
                    .thoughts = "",//DwarfGetThoughts(dwarf),
                    .profession = dwarf->profession(),
                    .unit_id = dwarf->unit_id(),
                    .pos_x = dwarf->pos_x(),
                    .pos_y = dwarf->pos_y(),
                    .pos_z = dwarf->pos_z(),
                    .happiness = dwarf->happiness(),
                    .flags1 = dwarf->flags1(),
                    .flags2 = dwarf->flags2(),
                    .flags3 = dwarf->flags3(),
                    .race = dwarf->race(),
                    .caste = dwarf->caste(),
                    .gender = dwarf->gender(),
                    .civ_id = dwarf->civ_id(),
                    .histfig_id = dwarf->histfig_id(),
                    .death_id = dwarf->death_id(),
                    .death_flags = dwarf->death_flags(),
                    .squad_id = dwarf->squad_id(),
                    .squad_position = dwarf->squad_position(),
                    .thought_string = "",  
                    .profession_name = "",
                    .current_job = "",
                    .old_labors_size = dwarf->labors_size(),
                    .labors = {},
                };
                for(int x = 0; x < dwarf->labors_size(); x++){
                    cdwarf.labors[x] = dwarf->labors(x);
                }
                cdwarves.push_back(cdwarf);
            } else {
                CDwarf* cdwarf = &cdwarves.at(j);
                cdwarf->Id = dwarf->unit_id();
                cdwarf->name = cstr;
                cdwarf->thoughts = "";//DwarfGetThoughts(dwarf);
                cdwarf->profession = dwarf->profession();
                cdwarf->unit_id = dwarf->unit_id();
                cdwarf->pos_x = dwarf->pos_x();
                cdwarf->pos_y = dwarf->pos_y();
                cdwarf->pos_z = dwarf->pos_z();
                cdwarf->happiness = dwarf->happiness();
                cdwarf->flags1 = dwarf->flags1();
                cdwarf->flags2 = dwarf->flags2();
                cdwarf->flags3 = dwarf->flags3();
                cdwarf->race = dwarf->race();
                cdwarf->caste = dwarf->caste();
                cdwarf->gender = dwarf->gender();
                cdwarf->civ_id = dwarf->civ_id();
                cdwarf->histfig_id = dwarf->histfig_id();
                std::fill_n(cdwarf->labors, cdwarf->old_labors_size, 0);
                cdwarf->old_labors_size = dwarf->labors_size();
                for(int x = 0; x < dwarf->labors_size(); x++){
                    cdwarf->labors[x] = dwarf->labors(x);
                }
            }
            j++;
        }
    }
    std::sort (dwarves.begin(), dwarves.end(), sort_dwarves);
    std::sort (cdwarves.begin(), cdwarves.end(), sort_cdwarves);
}

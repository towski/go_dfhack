#ifndef _MY_PACKAGE_FOO_H_
#define _MY_PACKAGE_FOO_H_


#ifdef __cplusplus
extern "C" {
#endif
#ifndef __cplusplus
    typedef enum { false, true } bool;
#endif
    
    struct SkillInfo{
    };

    struct UnitThought{
    };

    struct Labor{
        int unit_id;
        int labor;
        bool value;
    };

    struct CDwarf{
        int Id;
        const char* name;
        const char* thoughts;
        int profession;
        int unit_id;
        int pos_x;
        int pos_y;
        int pos_z;
        int happiness;
        unsigned int flags1;
        unsigned int flags2;
        unsigned int flags3;
        int race;
        int caste;
        int gender;

        int civ_id;
        int histfig_id;

        int death_id;
        unsigned int death_flags;

        int squad_id;
        int squad_position;


        char* thought_string;
        char* profession_name;
        char* current_job;

        int old_labors_size;
        int labors[100];

        struct SkillInfo* skills[100];
        struct UnitThought* recent_events[];
    };
    struct Enums{
        const char* names[100];
    };

    int Size();
    void Update();
    void Exit();
    int Paused();

#ifdef __cplusplus
}
#endif

#endif

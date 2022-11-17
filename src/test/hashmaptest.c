#include "tests.h"
#include "../util/hashmap.h"
#include "../util/string.h"
#include "../util/printf.h"

uint64_t debug_insert_hmap(struct hashmap * hmap, const char * str) {
    struct hashmap_data data;
    data.pointer = (void*)str;
    data.size = strlen(str);
    return insert(hmap, &data);
}

//TEST_SIZE Must match the number of strings in this array!
#define TEST_SIZE 2
const char * strings[] = {
    "68313842542344051942",
    "74562870368944795832",
    "02770865782488991112",
    "45843549000034200976",
    "34080141868015564682",
    "84922055350095824591",
    "68760704098537920674",
    "27510446920876797771",
    "84967884656169071266",
    "40079762918290078641",
    "25607698940355900897",
    "87143331215138259273",
    "72723378418679883128",
    "53904344863769449106",
    "71896284488494098737",
    "91865145393372766464",
    "33440774118212228537",
    "08708158842815848003",
    "62396953327705825530",
    "76079658053437839190",
    "38692277399757965140",
    "35042792244521019235",
    "19260555630218190340",
    "84244897654947034391",
    "99833125722772722036",
    "55952920417281702862",
    "40020537697584926893",
    "94684058332037852457",
    "45068550759026319683",
    "83567507333697478419",
    "60411181464967310832",
    "92423162240927414839",
    "31674922988188947211",
    "46381649807556390181",
    "62905243136160009860",
    "38742462669492179309",
    "73590811732568075297",
    "27303164028482242933",
    "25514101851815969256",
    "75264812283728944279",
    "13142558836466787440",
    "43423174286579555447",
    "31924354339594016085",
    "34590072149306912345",
    "23382566598135416569",
    "06223017172738703386",
    "62942080279157318074",
    "03390041098603354622",
    "90038690487408586928",
    "15934557077865355792"
};

void hashmaptest() {
    printf("[HASHMAPTEST] Starting test with %d items\n", TEST_SIZE);
    struct hashmap * hmap = create_hashmap(TEST_SIZE);

    uint64_t keys[TEST_SIZE];

    for (int i = 0; i < TEST_SIZE; i++) {
        keys[i] = debug_insert_hmap(hmap, strings[i]);
        if (keys[i] == 0) {
            printf("[HASHMAPTEST] Failed to insert %s at index %d\n", strings[i], i);
        }
    }

    for (int i = 0; i < TEST_SIZE; i++) {
        struct hashmap_data data = {0, 0};
        int result = retrieve(hmap, &data, keys[i]);
        if (result == 0) {
            printf("[HASHMAPTEST] Failed to retrieve index %d\n", i);
        }
        if (strcmp((char*)data.pointer, strings[i]) != 0) {
            printf("[HASHMAPTEST] Test fail index: %d\n", i);
            printf("[HASHMAPTEST] DATA: %s, SIZE: %d\n", (char*)data.pointer, data.size);
            printf("[HASHMAPTEST] Retrieved: %s, Expected: %s\n", (char*)data.pointer, strings[i]);
            return;
        }
    }

    printf("[HASHMAPTEST] !!!Test passed!!!\n");
}
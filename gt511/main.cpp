/*
    Library example for controlling the GT-511C3 Finger Print Scanner (FPS)
    Created by Josh Hawley, July 23rd 2013
    Licensed for non-commercial use, must include this license message
    basically, Feel free to hack away at it, but just give me credit for my work =)
    TLDR; Wil Wheaton's Law
*/

#include "FPS_GT511Linux.h"
#include "db.h"
#include <malloc.h>
#include "req.h"


#define GT511_PORT_ENTRY    ttyUSB0
#define GT511_PORT_EXIT     ttyUSB1
#define GT511_PORT_ENROLL   ttyUSB0


char * getline(void) {
    size_t lenmax = 100, len = lenmax;
    char * line = (char *)malloc(lenmax), * linep = line;
    int c;

    if(line == NULL)
        return NULL;

    for(;;) {
        c = fgetc(stdin);
        if(c == EOF)
            break;

        if(--len == 0) {
            len = lenmax;
            char * linen = (char *)realloc(linep, lenmax *= 2);

            if(linen == NULL) {
                free(linep);
                return NULL;
            }
            line = linen + (line - linep);
            linep = linen;
        }

        if((*line++ = c) == '\n')
            break;
    }
    *(line-1) = '\0';
    return linep;
}


int enroll() {
    int port = GT511_PORT_ENROLL;
    int baud = 9600;
    char mode[] = {'8', 'N', '1', 0};

    FPS_GT511 fps(port, baud, mode);
    usleep(100 * 1000); // wait for device init
    fps.Open();
    fps.UseSerialDebug = false;
    fps.SetLED(true);

    // =============================
    // ask for user ID
    printf("Welcome to user creation and enrollment.\nPlease input user email and press return:\n");
    char * email = getline();

    int userid = db_get_user_id(email);
    if(userid == -1) {
        userid = db_insert_user(email);
        if(userid == -1) {
            printf("ERROR: Could not create user !\n");
            return 1;
        }
        else {
            printf("User created with ID: %d\n", userid);
        }
    }
    else {
        printf("User already exists with ID: %d\n", userid);
    }


    // =============================
    // find open enroll id
    int enrollid = 0;
    bool usedid = true;
    while (usedid == true)
    {
        usedid = fps.CheckEnrolled(enrollid);
        if (usedid == true) enrollid++;
    }
    fps.EnrollStart(enrollid);

    // enroll
    printf("Enroll ID for '%s'' will be %d.\n", email, enrollid);
    printf("Press finger to Enroll\n");

    while(fps.IsPressFinger() == false)
        usleep(100 * 1000);
    bool bret = fps.CaptureFinger(true);
    int iret = 0;
    if (bret != false)
    {
        printf("Remove finger\n");
        fps.Enroll1();
        while(fps.IsPressFinger() == true)
            usleep(100 * 1000);
        printf("Press same finger again\n");
        while(fps.IsPressFinger() == false)
            usleep(100 * 1000);
        bret = fps.CaptureFinger(true);
        if (bret != false)
        {
            printf("Remove finger\n");
            fps.Enroll2();
            while(fps.IsPressFinger() == true)
                usleep(100 * 1000);
            printf("Press same finger yet again\n");
            while(fps.IsPressFinger() == false)
                usleep(100 * 1000);
            bret = fps.CaptureFinger(true);
            if (bret != false)
            {
                printf("Remove finger\n");
                iret = fps.Enroll3();
                if (iret == 0)
                {
                    printf("Enrolling Successful :)\n");
                    printf("Binding UserID %d and Fingerprint ID %d...\n", userid, enrollid);
                    db_insert_fingerprint(userid, enrollid, 0, NULL);
                    db_insert_event(enrollid, EEventTypeEnroll, true);
                    printf("Done.");
                }
                else
                {
                    printf("Enrolling Failed with error code: %d\n", iret);
                    db_insert_event(enrollid, EEventTypeEnroll, false);
                }
            }
            else printf("Failed to capture third finger\n");
        }
        else printf("Failed to capture second finger\n");
    }
    else printf("Failed to capture first finger\n");

    // loop
//    while(1) {
//        usleep(60 * 1000 * 1000);
//    }
    return 0;
}


void scan(int aPort) {

    int port = aPort;
    int baud = 9600;
    char mode[] = {'8', 'N', '1', 0};

    // setup
    FPS_GT511 fps(port, baud, mode);
    usleep(100 * 1000);
    fps.Open();
    fps.UseSerialDebug=false;
    usleep(100 * 1000);

    fps.SetLED(true);

    // loop
    while(1) {
        if(fps.IsPressFinger()) {
            fps.CaptureFinger(false);
            int id = fps.Identify1_N();
            if(id < MAX_FGP_COUNT) {
                printf("Verified ID: %d\n", id);
                db_insert_event(id, aPort == GT511_PORT_ENTRY ? EEventTypeEntry : EEventTypeExit, true);
            }
            else {
                printf("Finger not found\n");
                db_insert_event(-1, aPort == GT511_PORT_ENTRY ? EEventTypeEntry : EEventTypeExit, false);
            }
        }
        else {
            //printf("Please press finger\n");
        }
        usleep(100 * 1000);
    }
}


/*
 * Used for testing communication w/ module.
 */
void blink(int aPort) {
    int baud = 9600;
    char mode[] = {'8', 'N', '1', 0};

    // setup
    FPS_GT511 fps(aPort, baud, mode);
    usleep(100 * 1000);
    fps.Open();
    usleep(100 * 1000);

    // loop
    while(1) {
        fps.SetLED(true);
        usleep(500 * 1000);
        fps.SetLED(false);
        usleep(500 * 1000);
    }
}

int main() {
    // init sqlite and curl
    db_open();
    req_init();

//    int baud = 9600;
//    char mode[] = {'8', 'N', '1', 0};
//    FPS_GT511 fps(GT511_PORT_ENROLL, baud, mode);
//    usleep(100 * 1000);
//    fps.Open();
//    usleep(100 * 1000);
//    fps.DeleteAll();


    enroll();

    // TODO: pthread this ?
    scan(GT511_PORT_ENTRY);

    req_cleanup();
    db_close();

}

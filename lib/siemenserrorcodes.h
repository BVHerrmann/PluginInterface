#ifndef SIEMENSERRORCODES_H
#define SIEMENSERRORCODES_H

#include <QString>

namespace Siemens
{
namespace Sinamics
{

QString errorDescription(uint16_t errorCode)
{
    switch (errorCode)
    {
        case 1000:
            /* F01000 - Internal software error */
            return QObject::tr("F01000 - Internal software error");
            break;
        case 1001:
            /* F01001 - FloatingPoint exception */
            return QObject::tr("F01001 - FloatingPoint exception");
            break;
        case 1002:
            /* F01002 - Internal software error */
            return QObject::tr("F01002 - Internal software error");
            break;
        case 1003:
            /* F01003 - Acknowledgment delay when accessing the memory */
            return QObject::tr("F01003 - Acknowledgment delay when accessing the memory");
            break;
        case 1004:
            /* N01004 (F, A) - Internal software error */
            return QObject::tr("N01004 (F, A) - Internal software error");
            break;
        case 1005:
            /* F01005 - Firmware download for DRIVE-CLiQ component unsuccessful */
            return QObject::tr("F01005 - Firmware download for DRIVE-CLiQ component unsuccessful");
            break;
        case 1006:
            /* A01006 - Firmware update for DRIVE-CLiQ component required */
            return QObject::tr("A01006 - Firmware update for DRIVE-CLiQ component required");
            break;
        case 1007:
            /* A01007 - POWER ON for DRIVE-CLiQ component required */
            return QObject::tr("A01007 - POWER ON for DRIVE-CLiQ component required");
            break;
        case 1009:
            /* A01009 (N) - CU: Control module overtemperature */
            return QObject::tr("A01009 (N) - CU: Control module overtemperature");
            break;
        case 1010:
            /* F01010 - Drive type unknown */
            return QObject::tr("F01010 - Drive type unknown");
            break;
        case 1011:
            /* F01011 (N) - Download interrupted */
            return QObject::tr("F01011 (N) - Download interrupted");
            break;
        case 1012:
            /* F01012 (N) - Project conversion error */
            return QObject::tr("F01012 (N) - Project conversion error");
            break;
        case 1013:
            /* A01013 - CU: Fan operating time reached or exceeded */
            return QObject::tr("A01013 - CU: Fan operating time reached or exceeded");
            break;
        case 1014:
            /* F01014 - Topology: DRIVE-CLiQ component property changed */
            return QObject::tr("F01014 - Topology: DRIVE-CLiQ component property changed");
            break;
        case 1015:
            /* F01015 - Internal software error */
            return QObject::tr("F01015 - Internal software error");
            break;
        case 1016:
            /* A01016 (F) - Firmware changed */
            return QObject::tr("A01016 (F) - Firmware changed");
            break;
        case 1017:
            /* A01017 - Component lists changed */
            return QObject::tr("A01017 - Component lists changed");
            break;
        case 1020:
            /* A01020 - Writing to RAM disk unsuccessful */
            return QObject::tr("A01020 - Writing to RAM disk unsuccessful");
            break;
        case 1023:
            /* F01023 - Software timeout (internal) */
            return QObject::tr("F01023 - Software timeout (internal)");
            break;
        case 1030:
            /* F01030 - Sign-of-life failure for master control */
            return QObject::tr("F01030 - Sign-of-life failure for master control");
            break;
        case 1031:
            /* F01031 - Sign-of-life failure for OFF in REMOTE */
            return QObject::tr("F01031 - Sign-of-life failure for OFF in REMOTE");
            break;
        case 1032:
            /* A01032 (F) - ACX: all parameters must be saved */
            return QObject::tr("A01032 (F) - ACX: all parameters must be saved");
            break;
        case 1033:
            /* F01033 - Units changeover: Reference parameter value invalid */
            return QObject::tr("F01033 - Units changeover: Reference parameter value invalid");
            break;
        case 1034:
            /* F01034 - Units changeover: Calculation parameter values after reference value change unsuccessful */
            return QObject::tr("F01034 - Units changeover: Calculation parameter values after reference value change unsuccessful");
            break;
        case 1035:
            /* A01035 (F) - ACX: Parameter back-up file corrupted */
            return QObject::tr("A01035 (F) - ACX: Parameter back-up file corrupted");
            break;
        case 1036:
            /* F01036 (A) - ACX: Parameter back-up file missing */
            return QObject::tr("F01036 (A) - ACX: Parameter back-up file missing");
            break;
        case 1038:
            /* F01038 (A) - ACX: Loading the parameter back-up file unsuccessful */
            return QObject::tr("F01038 (A) - ACX: Loading the parameter back-up file unsuccessful");
            break;
        case 1039:
            /* F01039 (A) - ACX: Writing to the parameter back-up file was unsuccessful */
            return QObject::tr("F01039 (A) - ACX: Writing to the parameter back-up file was unsuccessful");
            break;
        case 1040:
            /* F01040 - Save parameter settings and carry out a POWER ON */
            return QObject::tr("F01040 - Save parameter settings and carry out a POWER ON");
            break;
        case 1041:
            /* F01041 - Parameter save necessary */
            return QObject::tr("F01041 - Parameter save necessary");
            break;
        case 1042:
            /* F01042 - Parameter error during project download */
            return QObject::tr("F01042 - Parameter error during project download");
            break;
        case 1043:
            /* F01043 - Fatal error at project download */
            return QObject::tr("F01043 - Fatal error at project download");
            break;
        case 1044:
            /* F01044 - CU: Descriptive data error */
            return QObject::tr("F01044 - CU: Descriptive data error");
            break;
        case 1045:
            /* A01045 - CU: Configuring data invalid */
            return QObject::tr("A01045 - CU: Configuring data invalid");
            break;
        case 1049:
            /* A01049 - CU: It is not possible to write to file */
            return QObject::tr("A01049 - CU: It is not possible to write to file");
            break;
        case 1050:
            /* F01050 - Memory card and device incompatible */
            return QObject::tr("F01050 - Memory card and device incompatible");
            break;
        case 1054:
            /* F01054 - CU: System limit exceeded */
            return QObject::tr("F01054 - CU: System limit exceeded");
            break;
        case 1055:
            /* F01055 - CU: Internal error (SYNO of port and application not identical) */
            return QObject::tr("F01055 - CU: Internal error (SYNO of port and application not identical)");
            break;
        case 1056:
            /* F01056 - CU: Internal error (clock cycle of parameter group already assigned differently) */
            return QObject::tr("F01056 - CU: Internal error (clock cycle of parameter group already assigned differently)");
            break;
        case 1057:
            /* F01057 - CU: Internal error (different DRIVE-CLiQ type for the slave) */
            return QObject::tr("F01057 - CU: Internal error (different DRIVE-CLiQ type for the slave)");
            break;
        case 1058:
            /* F01058 - CU: Internal error (slave missing in topology) */
            return QObject::tr("F01058 - CU: Internal error (slave missing in topology)");
            break;
        case 1059:
            /* F01059 - CU: Internal error (port does not exist) */
            return QObject::tr("F01059 - CU: Internal error (port does not exist)");
            break;
        case 1060:
            /* F01060 - CU: Internal error (parameter group not available) */
            return QObject::tr("F01060 - CU: Internal error (parameter group not available)");
            break;
        case 1061:
            /* F01061 - CU: Internal error (application not known) */
            return QObject::tr("F01061 - CU: Internal error (application not known)");
            break;
        case 1063:
            /* F01063 - CU: Internal error (PDM) */
            return QObject::tr("F01063 - CU: Internal error (PDM)");
            break;
        case 1064:
            /* A01064 (F) - CU: Internal error (CRC) */
            return QObject::tr("A01064 (F) - CU: Internal error (CRC)");
            break;
        case 1068:
            /* F01068 - CU: Data memory memory overflow */
            return QObject::tr("F01068 - CU: Data memory memory overflow");
            break;
        case 1069:
            /* A01069 - Parameter backup and device incompatible */
            return QObject::tr("A01069 - Parameter backup and device incompatible");
            break;
        case 1070:
            /* F01070 - Project/firmware is being downloaded to the memory card */
            return QObject::tr("F01070 - Project/firmware is being downloaded to the memory card");
            break;
        case 1072:
            /* F01072 - Memory card restored from the backup copy */
            return QObject::tr("F01072 - Memory card restored from the backup copy");
            break;
        case 1073:
            /* A01073 (N) - POWER ON required for backup copy on memory card */
            return QObject::tr("A01073 (N) - POWER ON required for backup copy on memory card");
            break;
        case 1082:
            /* F01082 - Parameter error when powering up from data backup */
            return QObject::tr("F01082 - Parameter error when powering up from data backup");
            break;
        case 1097:
            /* A01097 (N) - NTP server cannot be accessed */
            return QObject::tr("A01097 (N) - NTP server cannot be accessed");
            break;
        case 1099:
            /* A01099 (N) - UTC synchronization tolerance violated */
            return QObject::tr("A01099 (N) - UTC synchronization tolerance violated");
            break;
        case 1100:
            /* A01100 - CU: Memory card withdrawn */
            return QObject::tr("A01100 - CU: Memory card withdrawn");
            break;
        case 1105:
            /* F01105 (A) - CU: Insufficient memory */
            return QObject::tr("F01105 (A) - CU: Insufficient memory");
            break;
        case 1106:
            /* F01106 - CU: Insufficient memory */
            return QObject::tr("F01106 - CU: Insufficient memory");
            break;
        case 1107:
            /* F01107 - CU: Save to memory card unsuccessful */
            return QObject::tr("F01107 - CU: Save to memory card unsuccessful");
            break;
        case 1110:
            /* F01110 - CU: More than one SINAMICS G on one Control Unit */
            return QObject::tr("F01110 - CU: More than one SINAMICS G on one Control Unit");
            break;
        case 1111:
            /* F01111 - CU: Mixed operation of drive units illegal */
            return QObject::tr("F01111 - CU: Mixed operation of drive units illegal");
            break;
        case 1112:
            /* F01112 - CU: Power unit not permissible */
            return QObject::tr("F01112 - CU: Power unit not permissible");
            break;
        case 1120:
            /* F01120 (A) - Terminal initialization has failed */
            return QObject::tr("F01120 (A) - Terminal initialization has failed");
            break;
        case 1122:
            /* F01122 (A) - Frequency at the measuring probe input too high */
            return QObject::tr("F01122 (A) - Frequency at the measuring probe input too high");
            break;
        case 1123:
            /* F01123 - Power unit does not support digital inputs/outputs */
            return QObject::tr("F01123 - Power unit does not support digital inputs/outputs");
            break;
        case 1150:
            /* F01150 - CU: Number of instances of a drive object type exceeded */
            return QObject::tr("F01150 - CU: Number of instances of a drive object type exceeded");
            break;
        case 1151:
            /* F01151 - CU: Number of drive objects of a category exceeded */
            return QObject::tr("F01151 - CU: Number of drive objects of a category exceeded");
            break;
        case 1152:
            /* F01152 - CU: Invalid constellation of drive object types */
            return QObject::tr("F01152 - CU: Invalid constellation of drive object types");
            break;
        case 1200:
            /* F01200 - CU: Time slice management internal software error */
            return QObject::tr("F01200 - CU: Time slice management internal software error");
            break;
        case 1205:
            /* F01205 - CU: Time slice overflow */
            return QObject::tr("F01205 - CU: Time slice overflow");
            break;
        case 1221:
            /* F01221 - CU: Basic clock cycle too low */
            return QObject::tr("F01221 - CU: Basic clock cycle too low");
            break;
        case 1222:
            /* F01222 - CU: Basic clock cycle too low (computing time for communication not available) */
            return QObject::tr("F01222 - CU: Basic clock cycle too low (computing time for communication not available)");
            break;
        case 1223:
            /* A01223 - CU: Sampling time inconsistent */
            return QObject::tr("A01223 - CU: Sampling time inconsistent");
            break;
        case 1224:
            /* A01224 - CU: Pulse frequency inconsistent */
            return QObject::tr("A01224 - CU: Pulse frequency inconsistent");
            break;
        case 1250:
            /* F01250 - CU: CU-EEPROM incorrect read-only data */
            return QObject::tr("F01250 - CU: CU-EEPROM incorrect read-only data");
            break;
        case 1251:
            /* A01251 - CU: CU-EEPROM incorrect read-write data */
            return QObject::tr("A01251 - CU: CU-EEPROM incorrect read-write data");
            break;
        case 1255:
            /* F01255 - CU: Option Board EEPROM read-only data error */
            return QObject::tr("F01255 - CU: Option Board EEPROM read-only data error");
            break;
        case 1256:
            /* A01256 - CU: Option Board EEPROM read-write data error */
            return QObject::tr("A01256 - CU: Option Board EEPROM read-write data error");
            break;
        case 1260:
            /* F01260 - Software not released */
            return QObject::tr("F01260 - Software not released");
            break;
        case 1275:
            /* F01275 - Hardware description error */
            return QObject::tr("F01275 - Hardware description error");
            break;
        case 1276:
            /* A01276 - Hardware description not fully compatible */
            return QObject::tr("A01276 - Hardware description not fully compatible");
            break;
        case 1302:
            /* A01302 - Error in the component trace */
            return QObject::tr("A01302 - Error in the component trace");
            break;
        case 1303:
            /* F01303 - Component does not support the required function */
            return QObject::tr("F01303 - Component does not support the required function");
            break;
        case 1304:
            /* A01304 (F) - Firmware version of DRIVE-CLiQ component is not up-to-date */
            return QObject::tr("A01304 (F) - Firmware version of DRIVE-CLiQ component is not up-to-date");
            break;
        case 1305:
            /* F01305 - Topology: Component number missing */
            return QObject::tr("F01305 - Topology: Component number missing");
            break;
        case 1306:
            /* A01306 - Firmware of the DRIVE-CLiQ component being updated */
            return QObject::tr("A01306 - Firmware of the DRIVE-CLiQ component being updated");
            break;
        case 1314:
            /* A01314 - Topology: Component must not be present */
            return QObject::tr("A01314 - Topology: Component must not be present");
            break;
        case 1317:
            /* A01317 (N) - Deactivated component again present */
            return QObject::tr("A01317 (N) - Deactivated component again present");
            break;
        case 1318:
            /* A01318 - BICO: Deactivated interconnections present */
            return QObject::tr("A01318 - BICO: Deactivated interconnections present");
            break;
        case 1319:
            /* A01319 - Inserted component not initialized */
            return QObject::tr("A01319 - Inserted component not initialized");
            break;
        case 1320:
            /* A01320 - Topology: Drive object number does not exist in configuration */
            return QObject::tr("A01320 - Topology: Drive object number does not exist in configuration");
            break;
        case 1321:
            /* A01321 - Topology: Drive object number does not exist in configuration */
            return QObject::tr("A01321 - Topology: Drive object number does not exist in configuration");
            break;
        case 1322:
            /* A01322 - Topology: Drive object number present twice in configuration */
            return QObject::tr("A01322 - Topology: Drive object number present twice in configuration");
            break;
        case 1323:
            /* A01323 - Topology: More than two partial lists created */
            return QObject::tr("A01323 - Topology: More than two partial lists created");
            break;
        case 1324:
            /* A01324 - Topology: Dummy drive object number incorrectly created */
            return QObject::tr("A01324 - Topology: Dummy drive object number incorrectly created");
            break;
        case 1325:
            /* F01325 - Topology: Component number not present in target topology */
            return QObject::tr("F01325 - Topology: Component number not present in target topology");
            break;
        case 1330:
            /* A01330 - Topology: Quick commissioning not possible */
            return QObject::tr("A01330 - Topology: Quick commissioning not possible");
            break;
        case 1331:
            /* A01331 - Topology: At least one component not assigned to a drive object */
            return QObject::tr("A01331 - Topology: At least one component not assigned to a drive object");
            break;
        case 1340:
            /* F01340 - Topology: Too many components on one line */
            return QObject::tr("F01340 - Topology: Too many components on one line");
            break;
        case 1341:
            /* F01341 - Topology: Maximum number of DRIVE-CLiQ components exceeded */
            return QObject::tr("F01341 - Topology: Maximum number of DRIVE-CLiQ components exceeded");
            break;
        case 1354:
            /* F01354 - Topology: Actual topology indicates an illegal component */
            return QObject::tr("F01354 - Topology: Actual topology indicates an illegal component");
            break;
        case 1355:
            /* F01355 - Topology: Actual topology changed */
            return QObject::tr("F01355 - Topology: Actual topology changed");
            break;
        case 1356:
            /* F01356 - Topology: There is a defective DRIVE-CLiQ component */
            return QObject::tr("F01356 - Topology: There is a defective DRIVE-CLiQ component");
            break;
        case 1357:
            /* F01357 - Topology: Two Control Units identified on the DRIVE-CLiQ line */
            return QObject::tr("F01357 - Topology: Two Control Units identified on the DRIVE-CLiQ line");
            break;
        case 1358:
            /* A01358 - Topology: Line termination not available */
            return QObject::tr("A01358 - Topology: Line termination not available");
            break;
        case 1359:
            /* F01359 - Topology: DRIVE-CLiQ performance not sufficient */
            return QObject::tr("F01359 - Topology: DRIVE-CLiQ performance not sufficient");
            break;
        case 1360:
            /* F01360 - Topology: Actual topology not permissible */
            return QObject::tr("F01360 - Topology: Actual topology not permissible");
            break;
        case 1361:
            /* A01361 - Topology: Actual topology contains SINUMERIK and SIMOTION components */
            return QObject::tr("A01361 - Topology: Actual topology contains SINUMERIK and SIMOTION components");
            break;
        case 1362:
            /* A01362 - Topology: Topology rule(s) broken */
            return QObject::tr("A01362 - Topology: Topology rule(s) broken");
            break;
        case 1375:
            /* F01375 - Topology: Connection duplicated between two components */
            return QObject::tr("F01375 - Topology: Connection duplicated between two components");
            break;
        case 1380:
            /* F01380 - Topology: Actual topology EEPROM defective */
            return QObject::tr("F01380 - Topology: Actual topology EEPROM defective");
            break;
        case 1381:
            /* A01381 - Topology: Motor Module incorrectly inserted */
            return QObject::tr("A01381 - Topology: Motor Module incorrectly inserted");
            break;
        case 1382:
            /* A01382 - Topology: Sensor Module incorrectly inserted */
            return QObject::tr("A01382 - Topology: Sensor Module incorrectly inserted");
            break;
        case 1383:
            /* A01383 - Topology: Terminal Module incorrectly inserted */
            return QObject::tr("A01383 - Topology: Terminal Module incorrectly inserted");
            break;
        case 1384:
            /* A01384 - Topology: DRIVE-CLiQ Hub Module incorrectly inserted */
            return QObject::tr("A01384 - Topology: DRIVE-CLiQ Hub Module incorrectly inserted");
            break;
        case 1385:
            /* A01385 - Topology: Controller Extension incorrectly inserted */
            return QObject::tr("A01385 - Topology: Controller Extension incorrectly inserted");
            break;
        case 1386:
            /* A01386 - Topology: DRIVE-CLiQ component incorrectly inserted */
            return QObject::tr("A01386 - Topology: DRIVE-CLiQ component incorrectly inserted");
            break;
        case 1389:
            /* A01389 - Topology: Motor with DRIVE-CLiQ incorrectly inserted */
            return QObject::tr("A01389 - Topology: Motor with DRIVE-CLiQ incorrectly inserted");
            break;
        case 1416:
            /* A01416 - Topology: Component additionally inserted */
            return QObject::tr("A01416 - Topology: Component additionally inserted");
            break;
        case 1420:
            /* A01420 - Topology: Component different */
            return QObject::tr("A01420 - Topology: Component different");
            break;
        case 1425:
            /* A01425 - Topology: Serial number different */
            return QObject::tr("A01425 - Topology: Serial number different");
            break;
        case 1428:
            /* A01428 - Topology: Incorrect connection used */
            return QObject::tr("A01428 - Topology: Incorrect connection used");
            break;
        case 1451:
            /* F01451 - Topology: Target topology is invalid */
            return QObject::tr("F01451 - Topology: Target topology is invalid");
            break;
        case 1481:
            /* A01481 (N) - Topology: Motor Module not connected */
            return QObject::tr("A01481 (N) - Topology: Motor Module not connected");
            break;
        case 1482:
            /* A01482 - Topology: Sensor Module not connected */
            return QObject::tr("A01482 - Topology: Sensor Module not connected");
            break;
        case 1483:
            /* A01483 - Topology: Terminal Module not connected */
            return QObject::tr("A01483 - Topology: Terminal Module not connected");
            break;
        case 1484:
            /* A01484 - Topology: DRIVE-CLiQ Hub Module not connected */
            return QObject::tr("A01484 - Topology: DRIVE-CLiQ Hub Module not connected");
            break;
        case 1485:
            /* A01485 - Topology: Controller Extension not connected */
            return QObject::tr("A01485 - Topology: Controller Extension not connected");
            break;
        case 1486:
            /* A01486 - Topology: DRIVE-CLiQ component not connected */
            return QObject::tr("A01486 - Topology: DRIVE-CLiQ component not connected");
            break;
        case 1487:
            /* A01487 - Topology: Option slot component not inserted */
            return QObject::tr("A01487 - Topology: Option slot component not inserted");
            break;
        case 1489:
            /* A01489 - Topology: motor with DRIVE-CLiQ not connected */
            return QObject::tr("A01489 - Topology: motor with DRIVE-CLiQ not connected");
            break;
        case 1507:
            /* A01507 (F, N) - BICO: Interconnections to inactive objects present */
            return QObject::tr("A01507 (F, N) - BICO: Interconnections to inactive objects present");
            break;
        case 1508:
            /* A01508 - BICO: Interconnections to inactive objects exceeded */
            return QObject::tr("A01508 - BICO: Interconnections to inactive objects exceeded");
            break;
        case 1510:
            /* F01510 - BICO: Signal source is not float type */
            return QObject::tr("F01510 - BICO: Signal source is not float type");
            break;
        case 1511:
            /* F01511 (A) - BICO: Interconnection with different scalings */
            return QObject::tr("F01511 (A) - BICO: Interconnection with different scalings");
            break;
        case 1512:
            /* F01512 - BICO: No scaling available */
            return QObject::tr("F01512 - BICO: No scaling available");
            break;
        case 1513:
            /* F01513 (N, A) - BICO: Interconnection cross DO with different scalings */
            return QObject::tr("F01513 (N, A) - BICO: Interconnection cross DO with different scalings");
            break;
        case 1514:
            /* A01514 (F) - BICO: Error when writing during a reconnect */
            return QObject::tr("A01514 (F) - BICO: Error when writing during a reconnect");
            break;
        case 1515:
            /* F01515 (A) - BICO: Writing to parameter not permitted as the master control is active */
            return QObject::tr("F01515 (A) - BICO: Writing to parameter not permitted as the master control is active");
            break;
        case 1590:
            /* A01590 (F) - Drive: Motor maintenance interval expired */
            return QObject::tr("A01590 (F) - Drive: Motor maintenance interval expired");
            break;
        case 1600:
            /* F01600 - SI P1 (CU): STOP A initiated */
            return QObject::tr("F01600 - SI P1 (CU): STOP A initiated");
            break;
        case 1611:
            /* F01611 (A) - SI P1 (CU): Defect in a monitoring channel */
            return QObject::tr("F01611 (A) - SI P1 (CU): Defect in a monitoring channel");
            break;
        case 1612:
            /* F01612 - SI P1 (CU): STO inputs for power units connected in parallel different */
            return QObject::tr("F01612 - SI P1 (CU): STO inputs for power units connected in parallel different");
            break;
        case 1620:
            /* N01620 (F, A) - SI P1 (CU): Safe Torque Off active */
            return QObject::tr("N01620 (F, A) - SI P1 (CU): Safe Torque Off active");
            break;
        case 1621:
            /* N01621 (F, A) - SI P1 (CU): Safe Stop 1 active */
            return QObject::tr("N01621 (F, A) - SI P1 (CU): Safe Stop 1 active");
            break;
        case 1625:
            /* F01625 - SI P1 (CU): Sign-of-life error in safety data */
            return QObject::tr("F01625 - SI P1 (CU): Sign-of-life error in safety data");
            break;
        case 1630:
            /* F01630 - SI P1 (CU): Brake control error */
            return QObject::tr("F01630 - SI P1 (CU): Brake control error");
            break;
        case 1631:
            /* A01631 (F, N) - SI P1 (CU): motor holding brake/SBC configuration not practical */
            return QObject::tr("A01631 (F, N) - SI P1 (CU): motor holding brake/SBC configuration not practical");
            break;
        case 1632:
            /* F01632 - SI P1 (CU): shutoff valve control/feedback signal error */
            return QObject::tr("F01632 - SI P1 (CU): shutoff valve control/feedback signal error");
            break;
        case 1640:
            /* F01640 (N, A) - SI P1 (CU): component replacement identified and acknowledgment/save required */
            return QObject::tr("F01640 (N, A) - SI P1 (CU): component replacement identified and acknowledgment/save required");
            break;
        case 1641:
            /* F01641 (N, A) - SI P1 (CU): component replacement identified and save required */
            return QObject::tr("F01641 (N, A) - SI P1 (CU): component replacement identified and save required");
            break;
        case 1649:
            /* F01649 - SI P1 (CU): Internal software error */
            return QObject::tr("F01649 - SI P1 (CU): Internal software error");
            break;
        case 1650:
            /* F01650 - SI P1 (CU): Acceptance test required */
            return QObject::tr("F01650 - SI P1 (CU): Acceptance test required");
            break;
        case 1651:
            /* F01651 - SI P1 (CU): Synchronization safety time slices unsuccessful */
            return QObject::tr("F01651 - SI P1 (CU): Synchronization safety time slices unsuccessful");
            break;
        case 1652:
            /* F01652 - SI P1 (CU): Illegal monitoring clock cycle */
            return QObject::tr("F01652 - SI P1 (CU): Illegal monitoring clock cycle");
            break;
        case 1653:
            /* F01653 - SI P1 (CU): PROFIBUS/PROFINET configuration error */
            return QObject::tr("F01653 - SI P1 (CU): PROFIBUS/PROFINET configuration error");
            break;
        case 1654:
            /* A01654 (F, N) - SI P1 (CU): Deviating PROFIsafe configuration */
            return QObject::tr("A01654 (F, N) - SI P1 (CU): Deviating PROFIsafe configuration");
            break;
        case 1655:
            /* F01655 - SI P1 (CU): Align monitoring functions */
            return QObject::tr("F01655 - SI P1 (CU): Align monitoring functions");
            break;
        case 1656:
            /* F01656 - SI CU: Parameter monitoring channel 2 error */
            return QObject::tr("F01656 - SI CU: Parameter monitoring channel 2 error");
            break;
        case 1657:
            /* F01657 - SI P1 (CU): PROFIsafe telegram number invalid */
            return QObject::tr("F01657 - SI P1 (CU): PROFIsafe telegram number invalid");
            break;
        case 1658:
            /* F01658 - SI P1 (CU): PROFIsafe telegram number differ */
            return QObject::tr("F01658 - SI P1 (CU): PROFIsafe telegram number differ");
            break;
        case 1659:
            /* F01659 - SI P1 (CU): Write request for parameter rejected */
            return QObject::tr("F01659 - SI P1 (CU): Write request for parameter rejected");
            break;
        case 1660:
            /* F01660 - SI P1 (CU): Safety-related functions not supported */
            return QObject::tr("F01660 - SI P1 (CU): Safety-related functions not supported");
            break;
        case 1661:
            /* F01661 - SI P1 (CU): Simulation of the safety inputs active */
            return QObject::tr("F01661 - SI P1 (CU): Simulation of the safety inputs active");
            break;
        case 1663:
            /* F01663 - SI P1 (CU): Copying the SI parameters rejected */
            return QObject::tr("F01663 - SI P1 (CU): Copying the SI parameters rejected");
            break;
        case 1664:
            /* F01664 - SI P1 (CU): No automatic firmware update */
            return QObject::tr("F01664 - SI P1 (CU): No automatic firmware update");
            break;
        case 1665:
            /* F01665 - SI P1 (CU): System is defective */
            return QObject::tr("F01665 - SI P1 (CU): System is defective");
            break;
        case 1666:
            /* A01666 (F) - SI Motion P1 (CU): Steady-state (static) 1 signal at the F-DI for safe acknowledgment */
            return QObject::tr("A01666 (F) - SI Motion P1 (CU): Steady-state (static) 1 signal at the F-DI for safe acknowledgment");
            break;
        case 1669:
            /* A01669 (F, N) - SI Motion: Unfavorable combination of motor and power unit */
            return QObject::tr("A01669 (F, N) - SI Motion: Unfavorable combination of motor and power unit");
            break;
        case 1670:
            /* F01670 - SI Motion: Invalid parameterization Sensor Module */
            return QObject::tr("F01670 - SI Motion: Invalid parameterization Sensor Module");
            break;
        case 1671:
            /* F01671 - SI Motion: Parameterization encoder error */
            return QObject::tr("F01671 - SI Motion: Parameterization encoder error");
            break;
        case 1672:
            /* F01672 - SI P1 (CU): Motor Module software/hardware incompatible */
            return QObject::tr("F01672 - SI P1 (CU): Motor Module software/hardware incompatible");
            break;
        case 1673:
            /* F01673 - SI Motion: Sensor Module software/hardware incompatible */
            return QObject::tr("F01673 - SI Motion: Sensor Module software/hardware incompatible");
            break;
        case 1674:
            /* F01674 - SI Motion P1 (CU): Safety function not supported by PROFIsafe telegram */
            return QObject::tr("F01674 - SI Motion P1 (CU): Safety function not supported by PROFIsafe telegram");
            break;
        case 1675:
            /* F01675 - SI Motion P1: settings in the PROFIBUS/PROFINET controller not permissible */
            return QObject::tr("F01675 - SI Motion P1: settings in the PROFIBUS/PROFINET controller not permissible");
            break;
        case 1679:
            /* F01679 - SI CU: Safety parameter settings and topology changed, warm restart/POWER ON required */
            return QObject::tr("F01679 - SI CU: Safety parameter settings and topology changed, warm restart/POWER ON required");
            break;
        case 1680:
            /* F01680 - SI Motion P1 (CU): Checksum error safety monitoring functions */
            return QObject::tr("F01680 - SI Motion P1 (CU): Checksum error safety monitoring functions");
            break;
        case 1681:
            /* F01681 - SI Motion P1 (CU): Incorrect parameter value */
            return QObject::tr("F01681 - SI Motion P1 (CU): Incorrect parameter value");
            break;
        case 1682:
            /* F01682 - SI Motion P1 (CU): Monitoring function not supported */
            return QObject::tr("F01682 - SI Motion P1 (CU): Monitoring function not supported");
            break;
        case 1683:
            /* F01683 - SI Motion P1 (CU): SOS/SLS enable missing */
            return QObject::tr("F01683 - SI Motion P1 (CU): SOS/SLS enable missing");
            break;
        case 1684:
            /* F01684 - SI Motion P1 (CU): Safely-Limited Position limit values interchanged */
            return QObject::tr("F01684 - SI Motion P1 (CU): Safely-Limited Position limit values interchanged");
            break;
        case 1685:
            /* F01685 - SI Motion P1 (CU): Safely-Limited Speed limit value too high */
            return QObject::tr("F01685 - SI Motion P1 (CU): Safely-Limited Speed limit value too high");
            break;
        case 1686:
            /* F01686 - SI Motion: Illegal parameterization cam position */
            return QObject::tr("F01686 - SI Motion: Illegal parameterization cam position");
            break;
        case 1687:
            /* F01687 - SI Motion: Illegal parameterization modulo value SCA (SN) */
            return QObject::tr("F01687 - SI Motion: Illegal parameterization modulo value SCA (SN)");
            break;
        case 1688:
            /* F01688 - SI Motion CU: Actual value synchronization not permissible */
            return QObject::tr("F01688 - SI Motion CU: Actual value synchronization not permissible");
            break;
        case 1689:
            /* C01689 - SI Motion: Axis re-configured */
            return QObject::tr("C01689 - SI Motion: Axis re-configured");
            break;
        case 1690:
            /* F01690 - SI Motion: Data save problem for the NVRAM */
            return QObject::tr("F01690 - SI Motion: Data save problem for the NVRAM");
            break;
        case 1691:
            /* A01691 (F) - SI Motion: Ti and To unsuitable for DP cycle */
            return QObject::tr("A01691 (F) - SI Motion: Ti and To unsuitable for DP cycle");
            break;
        case 1692:
            /* F01692 - SI Motion P1 (CU): Parameter value not permitted for encoderless */
            return QObject::tr("F01692 - SI Motion P1 (CU): Parameter value not permitted for encoderless");
            break;
        case 1693:
            /* A01693 (F) - SI P1 (CU): Safety parameter setting changed, warm restart/POWER ON required */
            return QObject::tr("A01693 (F) - SI P1 (CU): Safety parameter setting changed, warm restart/POWER ON required");
            break;
        case 1694:
            /* F01694 (A) - SI Motion CU: Firmware version Motor Module/Hydraulic Module older Control Unit */
            return QObject::tr("F01694 (A) - SI Motion CU: Firmware version Motor Module/Hydraulic Module older Control Unit");
            break;
        case 1695:
            /* A01695 (F) - SI Motion: Sensor Module was replaced */
            return QObject::tr("A01695 (F) - SI Motion: Sensor Module was replaced");
            break;
        case 1696:
            /* A01696 (F) - SI Motion: Test stop for the motion monitoring functions selected when booting */
            return QObject::tr("A01696 (F) - SI Motion: Test stop for the motion monitoring functions selected when booting");
            break;
        case 1697:
            /* A01697 (F) - SI Motion: Test stop for motion monitoring functions required */
            return QObject::tr("A01697 (F) - SI Motion: Test stop for motion monitoring functions required");
            break;
        case 1698:
            /* A01698 (F) - SI P1 (CU): Commissioning mode active */
            return QObject::tr("A01698 (F) - SI P1 (CU): Commissioning mode active");
            break;
        case 1699:
            /* A01699 (F) - SI P1 (CU): Test stop for STO required */
            return QObject::tr("A01699 (F) - SI P1 (CU): Test stop for STO required");
            break;
        case 1700:
            /* C01700 - SI Motion P1 (CU): STOP A initiated */
            return QObject::tr("C01700 - SI Motion P1 (CU): STOP A initiated");
            break;
        case 1701:
            /* C01701 - SI Motion P1 (CU): STOP B initiated */
            return QObject::tr("C01701 - SI Motion P1 (CU): STOP B initiated");
            break;
        case 1706:
            /* C01706 - SI Motion P1 (CU): SAM/SBR limit exceeded */
            return QObject::tr("C01706 - SI Motion P1 (CU): SAM/SBR limit exceeded");
            break;
        case 1707:
            /* C01707 - SI Motion P1 (CU): Tolerance for safe operating stop exceeded */
            return QObject::tr("C01707 - SI Motion P1 (CU): Tolerance for safe operating stop exceeded");
            break;
        case 1708:
            /* C01708 - SI Motion P1 (CU): STOP C initiated */
            return QObject::tr("C01708 - SI Motion P1 (CU): STOP C initiated");
            break;
        case 1709:
            /* C01709 - SI Motion P1 (CU): STOP D initiated */
            return QObject::tr("C01709 - SI Motion P1 (CU): STOP D initiated");
            break;
        case 1710:
            /* C01710 - SI Motion P1 (CU): STOP E initiated */
            return QObject::tr("C01710 - SI Motion P1 (CU): STOP E initiated");
            break;
        case 1711:
            /* C01711 - SI Motion P1 (CU): Defect in a monitoring channel */
            return QObject::tr("C01711 - SI Motion P1 (CU): Defect in a monitoring channel");
            break;
        case 1712:
            /* C01712 - SI Motion P1 (CU): Defect in F-IO processing */
            return QObject::tr("C01712 - SI Motion P1 (CU): Defect in F-IO processing");
            break;
        case 1714:
            /* C01714 - SI Motion P1 (CU): Safely-Limited Speed exceeded */
            return QObject::tr("C01714 - SI Motion P1 (CU): Safely-Limited Speed exceeded");
            break;
        case 1715:
            /* C01715 - SI Motion P1 (CU): Safely-Limited Position exceeded */
            return QObject::tr("C01715 - SI Motion P1 (CU): Safely-Limited Position exceeded");
            break;
        case 1716:
            /* C01716 - SI Motion P1 (CU): Tolerance for safe motion direction exceeded */
            return QObject::tr("C01716 - SI Motion P1 (CU): Tolerance for safe motion direction exceeded");
            break;
        case 1717:
            /* C01717 - SI Motion P1 (CU): SLA limit exceeded */
            return QObject::tr("C01717 - SI Motion P1 (CU): SLA limit exceeded");
            break;
        case 1730:
            /* C01730 - SI Motion P1 (CU): Reference block for dynamic Safely-Limited Speed invalid */
            return QObject::tr("C01730 - SI Motion P1 (CU): Reference block for dynamic Safely-Limited Speed invalid");
            break;
        case 1745:
            /* C01745 - SI Motion P1 (CU): Checking braking torque for the brake test */
            return QObject::tr("C01745 - SI Motion P1 (CU): Checking braking torque for the brake test");
            break;
        case 1750:
            /* C01750 - SI Motion P1 (CU): Hardware fault safety-relevant encoder */
            return QObject::tr("C01750 - SI Motion P1 (CU): Hardware fault safety-relevant encoder");
            break;
        case 1751:
            /* C01751 - SI Motion P1 (CU): Effectivity test fault safety-relevant encoder */
            return QObject::tr("C01751 - SI Motion P1 (CU): Effectivity test fault safety-relevant encoder");
            break;
        case 1752:
            /* C01752 - SI Motion P1 (CU): reference position invalid */
            return QObject::tr("C01752 - SI Motion P1 (CU): reference position invalid");
            break;
        case 1770:
            /* C01770 - SI Motion P1 (CU): Discrepancy error of the failsafe inputs/outputs */
            return QObject::tr("C01770 - SI Motion P1 (CU): Discrepancy error of the failsafe inputs/outputs");
            break;
        case 1772:
            /* A01772 - SI Motion P1 (CU): Test stop for failsafe digital outputs running */
            return QObject::tr("A01772 - SI Motion P1 (CU): Test stop for failsafe digital outputs running");
            break;
        case 1773:
            /* F01773 - SI Motion P1 (CU): Test stop failsafe digital output error */
            return QObject::tr("F01773 - SI Motion P1 (CU): Test stop failsafe digital output error");
            break;
        case 1774:
            /* A01774 - SI Motion P1 (CU): Test stop for failsafe digital outputs required */
            return QObject::tr("A01774 - SI Motion P1 (CU): Test stop for failsafe digital outputs required");
            break;
        case 1780:
            /* A01780 - SBT When selected, the brake is closed */
            return QObject::tr("A01780 - SBT When selected, the brake is closed");
            break;
        case 1781:
            /* A01781 - SBT brake opening time exceeded */
            return QObject::tr("A01781 - SBT brake opening time exceeded");
            break;
        case 1782:
            /* A01782 - SBT brake test incorrect control */
            return QObject::tr("A01782 - SBT brake test incorrect control");
            break;
        case 1783:
            /* A01783 - SBT brake closing time exceeded */
            return QObject::tr("A01783 - SBT brake closing time exceeded");
            break;
        case 1784:
            /* A01784 - SBT brake test canceled with fault */
            return QObject::tr("A01784 - SBT brake test canceled with fault");
            break;
        case 1785:
            /* A01785 - SBT brake test configuration error */
            return QObject::tr("A01785 - SBT brake test configuration error");
            break;
        case 1786:
            /* F01786 - SCC signal source changed */
            return QObject::tr("F01786 - SCC signal source changed");
            break;
        case 1787:
            /* F01787 - SBT motor type different */
            return QObject::tr("F01787 - SBT motor type different");
            break;
        case 1788:
            /* A01788 - SI: Automatic test stop waits for STO deselection via motion monitoring functions */
            return QObject::tr("A01788 - SI: Automatic test stop waits for STO deselection via motion monitoring functions");
            break;
        case 1789:
            /* A01789 - SI: Automatic test stop and brake test when test stop is selected not permitted */
            return QObject::tr("A01789 - SI: Automatic test stop and brake test when test stop is selected not permitted");
            break;
        case 1794:
            /* A01794 (N) - SI Motion: check modulo value for safe position via PROFIsafe */
            return QObject::tr("A01794 (N) - SI Motion: check modulo value for safe position via PROFIsafe");
            break;
        case 1795:
            /* A01795 - SI Motion P1 (CU): Wait time after exiting the safe pulse cancellation expired */
            return QObject::tr("A01795 - SI Motion P1 (CU): Wait time after exiting the safe pulse cancellation expired");
            break;
        case 1796:
            /* A01796 (F, N) - SI P1 (CU): Wait for communication */
            return QObject::tr("A01796 (F, N) - SI P1 (CU): Wait for communication");
            break;
        case 1797:
            /* C01797 - SI Motion P1 (CU): Axis not safely referenced */
            return QObject::tr("C01797 - SI Motion P1 (CU): Axis not safely referenced");
            break;
        case 1798:
            /* C01798 - SI Motion P1 (CU): Test stop for motion monitoring functions running */
            return QObject::tr("C01798 - SI Motion P1 (CU): Test stop for motion monitoring functions running");
            break;
        case 1799:
            /* C01799 - SI Motion P1 (CU): Acceptance test mode active */
            return QObject::tr("C01799 - SI Motion P1 (CU): Acceptance test mode active");
            break;
        case 1800:
            /* F01800 - DRIVE-CLiQ: Hardware/configuration error */
            return QObject::tr("F01800 - DRIVE-CLiQ: Hardware/configuration error");
            break;
        case 1839:
            /* A01839 - DRIVE-CLiQ diagnostics: cable fault to the component */
            return QObject::tr("A01839 - DRIVE-CLiQ diagnostics: cable fault to the component");
            break;
        case 1840:
            /* A01840 - SMI: Component found without motor data */
            return QObject::tr("A01840 - SMI: Component found without motor data");
            break;
        case 1900:
            /* A01900 (F) - PB/PN: Configuration telegram error */
            return QObject::tr("A01900 (F) - PB/PN: Configuration telegram error");
            break;
        case 1902:
            /* A01902 - PB/PN: clock cycle synchronous operation parameterization not permissible */
            return QObject::tr("A01902 - PB/PN: clock cycle synchronous operation parameterization not permissible");
            break;
        case 1903:
            /* A01903 (F) - COMM INT: Receive configuration data invalid */
            return QObject::tr("A01903 (F) - COMM INT: Receive configuration data invalid");
            break;
        case 1910:
            /* F01910 (N, A) - Fieldbus: setpoint timeout */
            return QObject::tr("F01910 (N, A) - Fieldbus: setpoint timeout");
            break;
        case 1911:
            /* F01911 (N, A) - PB/PN: clock cycle synchronous operation clock cycle failure */
            return QObject::tr("F01911 (N, A) - PB/PN: clock cycle synchronous operation clock cycle failure");
            break;
        case 1912:
            /* F01912 (N, A) - PB/PN: clock cycle synchronous operation sign-of-life failure */
            return QObject::tr("F01912 (N, A) - PB/PN: clock cycle synchronous operation sign-of-life failure");
            break;
        case 1913:
            /* F01913 (N, A) - COMM INT: Monitoring time sign-of-life expired */
            return QObject::tr("F01913 (N, A) - COMM INT: Monitoring time sign-of-life expired");
            break;
        case 1914:
            /* F01914 (N, A) - COMM INT: Monitoring time configuration expired */
            return QObject::tr("F01914 (N, A) - COMM INT: Monitoring time configuration expired");
            break;
        case 1915:
            /* F01915 (N, A) - PB/PN: clock cycle synchronous operation sign-of-life failure drive object 1 */
            return QObject::tr("F01915 (N, A) - PB/PN: clock cycle synchronous operation sign-of-life failure drive object 1");
            break;
        case 1920:
            /* A01920 (F) - PROFIBUS: Interruption cyclic connection */
            return QObject::tr("A01920 (F) - PROFIBUS: Interruption cyclic connection");
            break;
        case 1921:
            /* A01921 (F) - PROFIBUS: Receive setpoints after To */
            return QObject::tr("A01921 (F) - PROFIBUS: Receive setpoints after To");
            break;
        case 1925:
            /* A01925 (F) - Modbus TCP: connection interrupted */
            return QObject::tr("A01925 (F) - Modbus TCP: connection interrupted");
            break;
        case 1930:
            /* A01930 - PB/PN: current controller sampling time clock cycle synch. not equal */
            return QObject::tr("A01930 - PB/PN: current controller sampling time clock cycle synch. not equal");
            break;
        case 1931:
            /* A01931 - PB/PN: speed controller sampling time clock cycle synch. not equal */
            return QObject::tr("A01931 - PB/PN: speed controller sampling time clock cycle synch. not equal");
            break;
        case 1932:
            /* A01932 - PB/PN: clock cycle synchronization missing for DSC */
            return QObject::tr("A01932 - PB/PN: clock cycle synchronization missing for DSC");
            break;
        case 1940:
            /* A01940 - PB/PN: clock cycle synchronism not reached */
            return QObject::tr("A01940 - PB/PN: clock cycle synchronism not reached");
            break;
        case 1941:
            /* A01941 - PB/PN: clock cycle signal missing when establishing bus communication */
            return QObject::tr("A01941 - PB/PN: clock cycle signal missing when establishing bus communication");
            break;
        case 1943:
            /* A01943 - PB/PN: clock cycle signal error when establishing bus communication */
            return QObject::tr("A01943 - PB/PN: clock cycle signal error when establishing bus communication");
            break;
        case 1944:
            /* A01944 - PB/PN: sign-of-life synchronism not reached */
            return QObject::tr("A01944 - PB/PN: sign-of-life synchronism not reached");
            break;
        case 1945:
            /* A01945 - PROFIBUS: Connection to the Publisher failed */
            return QObject::tr("A01945 - PROFIBUS: Connection to the Publisher failed");
            break;
        case 1946:
            /* F01946 (A) - PROFIBUS: Connection to the Publisher aborted */
            return QObject::tr("F01946 (A) - PROFIBUS: Connection to the Publisher aborted");
            break;
        case 1950:
            /* F01950 (N, A) - PB/PN: clock cycle synchronous operation synchronization unsuccessful */
            return QObject::tr("F01950 (N, A) - PB/PN: clock cycle synchronous operation synchronization unsuccessful");
            break;
        case 1951:
            /* F01951 - CU SYNC: Synchronization application clock cycle missing */
            return QObject::tr("F01951 - CU SYNC: Synchronization application clock cycle missing");
            break;
        case 1952:
            /* F01952 - CU DRIVE-CLiQ: Synchronization of component not supported */
            return QObject::tr("F01952 - CU DRIVE-CLiQ: Synchronization of component not supported");
            break;
        case 1953:
            /* A01953 - CU SYNC: Synchronization not completed */
            return QObject::tr("A01953 - CU SYNC: Synchronization not completed");
            break;
        case 1954:
            /* F01954 - CU DRIVE-CLiQ: Synchronization unsuccessful */
            return QObject::tr("F01954 - CU DRIVE-CLiQ: Synchronization unsuccessful");
            break;
        case 1955:
            /* A01955 - CU DRIVE-CLiQ: Synchronization DO not completed */
            return QObject::tr("A01955 - CU DRIVE-CLiQ: Synchronization DO not completed");
            break;
        case 1970:
            /* A01970 - CBE25: cyclic connection interrupted */
            return QObject::tr("A01970 - CBE25: cyclic connection interrupted");
            break;
        case 1971:
            /* A01971 - CBE2x: maximum number of controllers exceeded */
            return QObject::tr("A01971 - CBE2x: maximum number of controllers exceeded");
            break;
        case 1979:
            /* A01979 - CBE2x: internal error for cyclic data transfer */
            return QObject::tr("A01979 - CBE2x: internal error for cyclic data transfer");
            break;
        case 1980:
            /* A01980 - PN: cyclic connection interrupted */
            return QObject::tr("A01980 - PN: cyclic connection interrupted");
            break;
        case 1981:
            /* A01981 - PN: Maximum number of controllers exceeded */
            return QObject::tr("A01981 - PN: Maximum number of controllers exceeded");
            break;
        case 1982:
            /* A01982 - PN: second controller missing */
            return QObject::tr("A01982 - PN: second controller missing");
            break;
        case 1983:
            /* A01983 - PN: system redundancy switchover running */
            return QObject::tr("A01983 - PN: system redundancy switchover running");
            break;
        case 1989:
            /* A01989 - PN: internal cyclic data transfer error */
            return QObject::tr("A01989 - PN: internal cyclic data transfer error");
            break;
        case 1990:
            /* A01990 (F) - USS: PZD configuration error */
            return QObject::tr("A01990 (F) - USS: PZD configuration error");
            break;
        case 2000:
            /* A02000 - Function generator: Start not possible */
            return QObject::tr("A02000 - Function generator: Start not possible");
            break;
        case 2005:
            /* A02005 - Function generator: Drive does not exist */
            return QObject::tr("A02005 - Function generator: Drive does not exist");
            break;
        case 2006:
            /* A02006 - Function generator: No drive specified for connection */
            return QObject::tr("A02006 - Function generator: No drive specified for connection");
            break;
        case 2007:
            /* A02007 - Function generator: Drive not SERVO / VECTOR / DC_CTRL */
            return QObject::tr("A02007 - Function generator: Drive not SERVO / VECTOR / DC_CTRL");
            break;
        case 2008:
            /* A02008 - Function generator: Drive specified a multiple number of times */
            return QObject::tr("A02008 - Function generator: Drive specified a multiple number of times");
            break;
        case 2009:
            /* A02009 - Function generator: Illegal mode */
            return QObject::tr("A02009 - Function generator: Illegal mode");
            break;
        case 2010:
            /* A02010 - Function generator: Speed setpoint from the drive is not zero */
            return QObject::tr("A02010 - Function generator: Speed setpoint from the drive is not zero");
            break;
        case 2011:
            /* A02011 - Function generator: The actual drive speed is not zero */
            return QObject::tr("A02011 - Function generator: The actual drive speed is not zero");
            break;
        case 2015:
            /* A02015 - Function generator: Drive enable signals missing */
            return QObject::tr("A02015 - Function generator: Drive enable signals missing");
            break;
        case 2016:
            /* A02016 - Function generator: Magnetizing running */
            return QObject::tr("A02016 - Function generator: Magnetizing running");
            break;
        case 2020:
            /* A02020 - Function generator: Parameter cannot be changed */
            return QObject::tr("A02020 - Function generator: Parameter cannot be changed");
            break;
        case 2025:
            /* A02025 - Function generator: Period too short */
            return QObject::tr("A02025 - Function generator: Period too short");
            break;
        case 2026:
            /* A02026 - Function generator: Pulse width too high */
            return QObject::tr("A02026 - Function generator: Pulse width too high");
            break;
        case 2030:
            /* A02030 - Function generator: Physical address equals zero */
            return QObject::tr("A02030 - Function generator: Physical address equals zero");
            break;
        case 2040:
            /* A02040 - Function generator: Illegal value for offset */
            return QObject::tr("A02040 - Function generator: Illegal value for offset");
            break;
        case 2041:
            /* A02041 - Function generator: Illegal value for bandwidth */
            return QObject::tr("A02041 - Function generator: Illegal value for bandwidth");
            break;
        case 2047:
            /* A02047 - Function generator: Time slice clock cycle invalid */
            return QObject::tr("A02047 - Function generator: Time slice clock cycle invalid");
            break;
        case 2050:
            /* A02050 - Trace: Start not possible */
            return QObject::tr("A02050 - Trace: Start not possible");
            break;
        case 2051:
            /* A02051 - Trace: recording not possible as a result of know-how protection */
            return QObject::tr("A02051 - Trace: recording not possible as a result of know-how protection");
            break;
        case 2055:
            /* A02055 - Trace: Recording time too short */
            return QObject::tr("A02055 - Trace: Recording time too short");
            break;
        case 2056:
            /* A02056 - Trace: Recording cycle too short */
            return QObject::tr("A02056 - Trace: Recording cycle too short");
            break;
        case 2057:
            /* A02057 - Trace: Time slice clock cycle invalid */
            return QObject::tr("A02057 - Trace: Time slice clock cycle invalid");
            break;
        case 2058:
            /* A02058 - Trace: Time slice clock cycle for endless trace not valid */
            return QObject::tr("A02058 - Trace: Time slice clock cycle for endless trace not valid");
            break;
        case 2059:
            /* A02059 - Trace: Time slice clock cycle for 2 x 8 recording channels not valid */
            return QObject::tr("A02059 - Trace: Time slice clock cycle for 2 x 8 recording channels not valid");
            break;
        case 2060:
            /* A02060 - Trace: Signal to be traced missing */
            return QObject::tr("A02060 - Trace: Signal to be traced missing");
            break;
        case 2061:
            /* A02061 - Trace: Invalid signal */
            return QObject::tr("A02061 - Trace: Invalid signal");
            break;
        case 2062:
            /* A02062 - Trace: Invalid trigger signal */
            return QObject::tr("A02062 - Trace: Invalid trigger signal");
            break;
        case 2063:
            /* A02063 - Trace: Invalid data type */
            return QObject::tr("A02063 - Trace: Invalid data type");
            break;
        case 2070:
            /* A02070 - Trace: Parameter cannot be changed */
            return QObject::tr("A02070 - Trace: Parameter cannot be changed");
            break;
        case 2075:
            /* A02075 - Trace: Pretrigger time too long */
            return QObject::tr("A02075 - Trace: Pretrigger time too long");
            break;
        case 2080:
            /* F02080 - Trace: Parameterization deleted due to unit changeover */
            return QObject::tr("F02080 - Trace: Parameterization deleted due to unit changeover");
            break;
        case 2085:
            /* A02085 - Message function: Parameterization error */
            return QObject::tr("A02085 - Message function: Parameterization error");
            break;
        case 2095:
            /* A02095 - MTrace 0: multiple trace cannot be activated */
            return QObject::tr("A02095 - MTrace 0: multiple trace cannot be activated");
            break;
        case 2096:
            /* A02096 - MTrace 0: cannot be saved */
            return QObject::tr("A02096 - MTrace 0: cannot be saved");
            break;
        case 2097:
            /* A02097 - MTrace 1: multiple trace cannot be activated */
            return QObject::tr("A02097 - MTrace 1: multiple trace cannot be activated");
            break;
        case 2098:
            /* A02098 - MTrace 1: cannot be saved */
            return QObject::tr("A02098 - MTrace 1: cannot be saved");
            break;
        case 2099:
            /* A02099 - Trace: Insufficient Control Unit memory */
            return QObject::tr("A02099 - Trace: Insufficient Control Unit memory");
            break;
        case 2100:
            /* A02100 - Drive: Computing dead time current controller too short */
            return QObject::tr("A02100 - Drive: Computing dead time current controller too short");
            break;
        case 2150:
            /* A02150 - TEC: Technology Extension cannot be loaded */
            return QObject::tr("A02150 - TEC: Technology Extension cannot be loaded");
            break;
        case 2151:
            /* F02151 (A) - TEC: internal software error */
            return QObject::tr("F02151 (A) - TEC: internal software error");
            break;
        case 2152:
            /* F02152 (A) - TEC: insufficient memory */
            return QObject::tr("F02152 (A) - TEC: insufficient memory");
            break;
        case 2153:
            /* F02153 - TEC: technology function does not exist */
            return QObject::tr("F02153 - TEC: technology function does not exist");
            break;
        case 3000:
            /* F03000 - NVRAM fault on action */
            return QObject::tr("F03000 - NVRAM fault on action");
            break;
        case 3001:
            /* F03001 - NVRAM checksum incorrect */
            return QObject::tr("F03001 - NVRAM checksum incorrect");
            break;
        case 3500:
            /* F03500 (A) - TM: Initialization */
            return QObject::tr("F03500 (A) - TM: Initialization");
            break;
        case 3501:
            /* A03501 - TM: Sampling time change */
            return QObject::tr("A03501 - TM: Sampling time change");
            break;
        case 3505:
            /* F03505 (N, A) - TM: Analog input wire breakage */
            return QObject::tr("F03505 (N, A) - TM: Analog input wire breakage");
            break;
        case 3506:
            /* A03506 (F, N) - 24 V power supply missing */
            return QObject::tr("A03506 (F, N) - 24 V power supply missing");
            break;
        case 3507:
            /* A03507 (F, N) - Digital output not set */
            return QObject::tr("A03507 (F, N) - Digital output not set");
            break;
        case 3510:
            /* A03510 (F, N) - TM: Calibration data not plausible */
            return QObject::tr("A03510 (F, N) - TM: Calibration data not plausible");
            break;
        case 3550:
            /* A03550 - TM: Speed setpoint filter natural frequency > Shannon frequency */
            return QObject::tr("A03550 - TM: Speed setpoint filter natural frequency > Shannon frequency");
            break;
        case 3590:
            /* F03590 (N, A) - TM: Module not ready */
            return QObject::tr("F03590 (N, A) - TM: Module not ready");
            break;
        case 5000:
            /* A05000 (N) - Power unit: Overtemperature heat sink AC inverter */
            return QObject::tr("A05000 (N) - Power unit: Overtemperature heat sink AC inverter");
            break;
        case 5001:
            /* A05001 (N) - Power unit: Overtemperature depletion layer chip */
            return QObject::tr("A05001 (N) - Power unit: Overtemperature depletion layer chip");
            break;
        case 5002:
            /* A05002 (N) - Power unit: Air intake overtemperature */
            return QObject::tr("A05002 (N) - Power unit: Air intake overtemperature");
            break;
        case 5003:
            /* A05003 (N) - Power unit: Internal overtemperature */
            return QObject::tr("A05003 (N) - Power unit: Internal overtemperature");
            break;
        case 5004:
            /* A05004 (N) - Power unit: Rectifier overtemperature */
            return QObject::tr("A05004 (N) - Power unit: Rectifier overtemperature");
            break;
        case 5005:
            /* A05005 - Cooling unit: Cooling medium flow rate too low */
            return QObject::tr("A05005 - Cooling unit: Cooling medium flow rate too low");
            break;
        case 5006:
            /* A05006 (N) - Power unit: Overtemperature thermal model */
            return QObject::tr("A05006 (N) - Power unit: Overtemperature thermal model");
            break;
        case 5007:
            /* N05007 (A) - Power unit: Overtemperature thermal model (chassis PU) */
            return QObject::tr("N05007 (A) - Power unit: Overtemperature thermal model (chassis PU)");
            break;
        case 5050:
            /* F05050 - Parallel circuit: Pulse enable in spite of pulse inhibit */
            return QObject::tr("F05050 - Parallel circuit: Pulse enable in spite of pulse inhibit");
            break;
        case 5051:
            /* F05051 - Parallel circuit: Power unit pulse enable missing */
            return QObject::tr("F05051 - Parallel circuit: Power unit pulse enable missing");
            break;
        case 5052:
            /* A05052 (F) - Parallel circuit: Illegal current asymmetry */
            return QObject::tr("A05052 (F) - Parallel circuit: Illegal current asymmetry");
            break;
        case 5053:
            /* A05053 (F) - Parallel circuit: Inadmissible DC link voltage asymmetry */
            return QObject::tr("A05053 (F) - Parallel circuit: Inadmissible DC link voltage asymmetry");
            break;
        case 5054:
            /* A05054 (N) - Parallel circuit: Power unit deactivated */
            return QObject::tr("A05054 (N) - Parallel circuit: Power unit deactivated");
            break;
        case 5055:
            /* F05055 - Parallel connection: Power units with illegal code numbers */
            return QObject::tr("F05055 - Parallel connection: Power units with illegal code numbers");
            break;
        case 5056:
            /* F05056 - Parallel circuit: Power unit EEPROM versions differ */
            return QObject::tr("F05056 - Parallel circuit: Power unit EEPROM versions differ");
            break;
        case 5057:
            /* F05057 - Parallel circuit: Power unit firmware versions differ */
            return QObject::tr("F05057 - Parallel circuit: Power unit firmware versions differ");
            break;
        case 5058:
            /* F05058 - Parallel circuit: VSM EEPROM versions differ */
            return QObject::tr("F05058 - Parallel circuit: VSM EEPROM versions differ");
            break;
        case 5059:
            /* F05059 - Parallel circuit: VSM firmware versions differ */
            return QObject::tr("F05059 - Parallel circuit: VSM firmware versions differ");
            break;
        case 5060:
            /* F05060 - Parallel circuit: Power unit firmware version does not match */
            return QObject::tr("F05060 - Parallel circuit: Power unit firmware version does not match");
            break;
        case 5061:
            /* F05061 - Infeed VSM count */
            return QObject::tr("F05061 - Infeed VSM count");
            break;
        case 5064:
            /* F05064 - Parallel connection: Pulse synchronization error */
            return QObject::tr("F05064 - Parallel connection: Pulse synchronization error");
            break;
        case 5065:
            /* A05065 (F, N) - Voltage measured values not plausible */
            return QObject::tr("A05065 (F, N) - Voltage measured values not plausible");
            break;
        case 5118:
            /* F05118 (A) - Precharging contactor simultaneity monitoring time exceeded */
            return QObject::tr("F05118 (A) - Precharging contactor simultaneity monitoring time exceeded");
            break;
        case 5119:
            /* F05119 (A) - Bypass contactor simultaneity monitoring time exceeded */
            return QObject::tr("F05119 (A) - Bypass contactor simultaneity monitoring time exceeded");
            break;
        case 6000:
            /* F06000 - Infeed: Precharging monitoring time expired */
            return QObject::tr("F06000 - Infeed: Precharging monitoring time expired");
            break;
        case 6010:
            /* F06010 - Infeed: Power unit EP 24 V missing in operation */
            return QObject::tr("F06010 - Infeed: Power unit EP 24 V missing in operation");
            break;
        case 6050:
            /* F06050 - Infeed: Smart Mode not supported */
            return QObject::tr("F06050 - Infeed: Smart Mode not supported");
            break;
        case 6052:
            /* F06052 - Infeed: Filter temperature evaluation not supported */
            return QObject::tr("F06052 - Infeed: Filter temperature evaluation not supported");
            break;
        case 6080:
            /* F06080 (A) - Infeed: parameter error */
            return QObject::tr("F06080 (A) - Infeed: parameter error");
            break;
        case 6100:
            /* F06100 - Infeed: Shutdown due to line supply undervoltage condition */
            return QObject::tr("F06100 - Infeed: Shutdown due to line supply undervoltage condition");
            break;
        case 6105:
            /* A06105 (F) - Infeed: Line supply undervoltage */
            return QObject::tr("A06105 (F) - Infeed: Line supply undervoltage");
            break;
        case 6200:
            /* F06200 - Infeed: One or several line phases failed */
            return QObject::tr("F06200 - Infeed: One or several line phases failed");
            break;
        case 6205:
            /* A06205 (F) - Infeed: Voltage dip in at least one line supply phase */
            return QObject::tr("A06205 (F) - Infeed: Voltage dip in at least one line supply phase");
            break;
        case 6206:
            /* A06206 (F) - Infeed: Line currents asymmetrical alarm */
            return QObject::tr("A06206 (F) - Infeed: Line currents asymmetrical alarm");
            break;
        case 6207:
            /* F06207 (N, A) - Infeed: Line currents asymmetrical */
            return QObject::tr("F06207 (N, A) - Infeed: Line currents asymmetrical");
            break;
        case 6208:
            /* A06208 (F, N) - Infeed line: supply voltage asymmetrical */
            return QObject::tr("A06208 (F, N) - Infeed line: supply voltage asymmetrical");
            break;
        case 6210:
            /* F06210 - Infeed: Summation current too high */
            return QObject::tr("F06210 - Infeed: Summation current too high");
            break;
        case 6211:
            /* F06211 - Summation current impermissibly high */
            return QObject::tr("F06211 - Summation current impermissibly high");
            break;
        case 6215:
            /* A06215 (F) - Infeed: Summation current too high */
            return QObject::tr("A06215 (F) - Infeed: Summation current too high");
            break;
        case 6250:
            /* A06250 (F, N) - Infeed: Defective capacitor(s) in at least one phase of line filter */
            return QObject::tr("A06250 (F, N) - Infeed: Defective capacitor(s) in at least one phase of line filter");
            break;
        case 6255:
            /* F06255 (A) - Infeed: temperature threshold value not permissible */
            return QObject::tr("F06255 (A) - Infeed: temperature threshold value not permissible");
            break;
        case 6260:
            /* A06260 - Infeed: Temperature in the line filter too high */
            return QObject::tr("A06260 - Infeed: Temperature in the line filter too high");
            break;
        case 6261:
            /* F06261 - Infeed: Temperature in the line filter permanently too high */
            return QObject::tr("F06261 - Infeed: Temperature in the line filter permanently too high");
            break;
        case 6262:
            /* F06262 - Infeed: Temperature switch in the line filter open when switching on */
            return QObject::tr("F06262 - Infeed: Temperature switch in the line filter open when switching on");
            break;
        case 6300:
            /* F06300 - Infeed: Line voltage too high at power on */
            return QObject::tr("F06300 - Infeed: Line voltage too high at power on");
            break;
        case 6301:
            /* A06301 (F) - Infeed: Line supply overvoltage */
            return QObject::tr("A06301 (F) - Infeed: Line supply overvoltage");
            break;
        case 6310:
            /* F06310 (A) - Supply voltage (p0210) incorrectly parameterized */
            return QObject::tr("F06310 (A) - Supply voltage (p0210) incorrectly parameterized");
            break;
        case 6311:
            /* F06311 - Infeed: Supply voltage (p0210) incorrect */
            return QObject::tr("F06311 - Infeed: Supply voltage (p0210) incorrect");
            break;
        case 6320:
            /* F06320 - Master/slave: 4-channel multiplexer control not valid */
            return QObject::tr("F06320 - Master/slave: 4-channel multiplexer control not valid");
            break;
        case 6321:
            /* F06321 - Master/slave: 6-channel multiplexer control not valid */
            return QObject::tr("F06321 - Master/slave: 6-channel multiplexer control not valid");
            break;
        case 6350:
            /* A06350 (F) - Infeed: Measured line frequency too high */
            return QObject::tr("A06350 (F) - Infeed: Measured line frequency too high");
            break;
        case 6351:
            /* A06351 (F) - Infeed: Measured line frequency too low */
            return QObject::tr("A06351 (F) - Infeed: Measured line frequency too low");
            break;
        case 6400:
            /* A06400 - Infeed: Line supply data identification selected/active */
            return QObject::tr("A06400 - Infeed: Line supply data identification selected/active");
            break;
        case 6401:
            /* A06401 - Infeed: Transformer data identification/test mode selected/active */
            return QObject::tr("A06401 - Infeed: Transformer data identification/test mode selected/active");
            break;
        case 6500:
            /* F06500 - Infeed: Line synchronization not possible */
            return QObject::tr("F06500 - Infeed: Line synchronization not possible");
            break;
        case 6502:
            /* A06502 (F, N) - Infeed: Unable to achieve line synchronization in transformer magnetization */
            return QObject::tr("A06502 (F, N) - Infeed: Unable to achieve line synchronization in transformer magnetization");
            break;
        case 6503:
            /* F06503 - Infeed: Line black start unsuccessful */
            return QObject::tr("F06503 - Infeed: Line black start unsuccessful");
            break;
        case 6504:
            /* F06504 - Infeed: Island line supply synchronization unsuccessful */
            return QObject::tr("F06504 - Infeed: Island line supply synchronization unsuccessful");
            break;
        case 6505:
            /* F06505 - Infeed: For transformer magnetization current limit exceeded */
            return QObject::tr("F06505 - Infeed: For transformer magnetization current limit exceeded");
            break;
        case 6601:
            /* A06601 (F) - Infeed: Current offset measurement interrupted */
            return QObject::tr("A06601 (F) - Infeed: Current offset measurement interrupted");
            break;
        case 6602:
            /* A06602 (F) - Infeed: Current offset measurement not possible */
            return QObject::tr("A06602 (F) - Infeed: Current offset measurement not possible");
            break;
        case 6700:
            /* F06700 (A) - Infeed: Switch line contactor for load condition */
            return QObject::tr("F06700 (A) - Infeed: Switch line contactor for load condition");
            break;
        case 6800:
            /* A06800 (F) - Infeed: Maximum steady-state DC link voltage reached */
            return QObject::tr("A06800 (F) - Infeed: Maximum steady-state DC link voltage reached");
            break;
        case 6810:
            /* A06810 (F) - Infeed: DC link voltage alarm threshold fallen below */
            return QObject::tr("A06810 (F) - Infeed: DC link voltage alarm threshold fallen below");
            break;
        case 6849:
            /* A06849 (F, N) - Infeed: Short-circuit operation active */
            return QObject::tr("A06849 (F, N) - Infeed: Short-circuit operation active");
            break;
        case 6850:
            /* F06850 - Infeed: Short-circuit prevailing for too long */
            return QObject::tr("F06850 - Infeed: Short-circuit prevailing for too long");
            break;
        case 6851:
            /* F06851 - Infeed: Distributed infeed line monitoring tripped */
            return QObject::tr("F06851 - Infeed: Distributed infeed line monitoring tripped");
            break;
        case 6855:
            /* F06855 - Infeed: Line filter monitor responded */
            return QObject::tr("F06855 - Infeed: Line filter monitor responded");
            break;
        case 6860:
            /* A06860 - Infeed: Function module activation not possible */
            return QObject::tr("A06860 - Infeed: Function module activation not possible");
            break;
        case 6900:
            /* A06900 (F) - Braking Module: Fault (1 -> 0) */
            return QObject::tr("A06900 (F) - Braking Module: Fault (1 -> 0)");
            break;
        case 6901:
            /* A06901 - Braking Module: Pre-alarm I2t shutdown */
            return QObject::tr("A06901 - Braking Module: Pre-alarm I2t shutdown");
            break;
        case 6904:
            /* A06904 (N) - Braking Module internal is inhibited */
            return QObject::tr("A06904 (N) - Braking Module internal is inhibited");
            break;
        case 6905:
            /* A06905 - Braking Module internal I2t shutdown alarm */
            return QObject::tr("A06905 - Braking Module internal I2t shutdown alarm");
            break;
        case 6906:
            /* F06906 (A) - Braking Module internal fault */
            return QObject::tr("F06906 (A) - Braking Module internal fault");
            break;
        case 6907:
            /* F06907 - Braking Module internal overtemperature */
            return QObject::tr("F06907 - Braking Module internal overtemperature");
            break;
        case 6908:
            /* F06908 - Braking Module internal overtemperature shutdown */
            return QObject::tr("F06908 - Braking Module internal overtemperature shutdown");
            break;
        case 6909:
            /* F06909 - Braking Module internal Vce fault */
            return QObject::tr("F06909 - Braking Module internal Vce fault");
            break;
        case 6921:
            /* A06921 (N) - Braking resistor phase asymmetry */
            return QObject::tr("A06921 (N) - Braking resistor phase asymmetry");
            break;
        case 6922:
            /* F06922 - Braking resistor phase failure */
            return QObject::tr("F06922 - Braking resistor phase failure");
            break;
        case 7011:
            /* F07011 - Drive: Motor overtemperature */
            return QObject::tr("F07011 - Drive: Motor overtemperature");
            break;
        case 7012:
            /* A07012 (N) - Drive: Motor temperature model 1/3 overtemperature */
            return QObject::tr("A07012 (N) - Drive: Motor temperature model 1/3 overtemperature");
            break;
        case 7013:
            /* F07013 - Drive: Motor temperature model configuration fault */
            return QObject::tr("F07013 - Drive: Motor temperature model configuration fault");
            break;
        case 7014:
            /* A07014 (N) - Drive: Motor temperature model configuration alarm */
            return QObject::tr("A07014 (N) - Drive: Motor temperature model configuration alarm");
            break;
        case 7015:
            /* A07015 - Drive: Motor temperature sensor alarm */
            return QObject::tr("A07015 - Drive: Motor temperature sensor alarm");
            break;
        case 7016:
            /* F07016 - Drive: Motor temperature sensor fault */
            return QObject::tr("F07016 - Drive: Motor temperature sensor fault");
            break;
        case 7017:
            /* A07017 - Additional temperature alarm threshold exceeded */
            return QObject::tr("A07017 - Additional temperature alarm threshold exceeded");
            break;
        case 7018:
            /* F07018 - Additional temperature fault threshold exceeded */
            return QObject::tr("F07018 - Additional temperature fault threshold exceeded");
            break;
        case 7048:
            /* F07048 - Drive: Incorrect characteristic measurement */
            return QObject::tr("F07048 - Drive: Incorrect characteristic measurement");
            break;
        case 7080:
            /* F07080 - Drive: Incorrect control parameter */
            return QObject::tr("F07080 - Drive: Incorrect control parameter");
            break;
        case 7082:
            /* F07082 - Macro: Execution not possible */
            return QObject::tr("F07082 - Macro: Execution not possible");
            break;
        case 7083:
            /* F07083 - Macro: ACX file not found */
            return QObject::tr("F07083 - Macro: ACX file not found");
            break;
        case 7084:
            /* F07084 - Macro: Condition for WaitUntil not fulfilled */
            return QObject::tr("F07084 - Macro: Condition for WaitUntil not fulfilled");
            break;
        case 7085:
            /* F07085 - Drive: Open-loop/closed-loop control parameters changed */
            return QObject::tr("F07085 - Drive: Open-loop/closed-loop control parameters changed");
            break;
        case 7086:
            /* F07086 - Units changeover: Parameter limit violation due to reference value change */
            return QObject::tr("F07086 - Units changeover: Parameter limit violation due to reference value change");
            break;
        case 7087:
            /* F07087 - Drive: Encoderless operation not possible for the selected pulse frequency */
            return QObject::tr("F07087 - Drive: Encoderless operation not possible for the selected pulse frequency");
            break;
        case 7088:
            /* F07088 - Units changeover: Parameter limit violation due to units changeover */
            return QObject::tr("F07088 - Units changeover: Parameter limit violation due to units changeover");
            break;
        case 7089:
            /* A07089 - Changing over units: Function module activation is blocked because the units have been changed over */
            return QObject::tr("A07089 - Changing over units: Function module activation is blocked because the units have been changed over");
            break;
        case 7090:
            /* F07090 - Drive: Upper torque limit less than the lower torque limit */
            return QObject::tr("F07090 - Drive: Upper torque limit less than the lower torque limit");
            break;
        case 7091:
            /* A07091 - Drive: determined current controller dynamic response invalid */
            return QObject::tr("A07091 - Drive: determined current controller dynamic response invalid");
            break;
        case 7092:
            /* A07092 - Drive: moment of inertia estimator still not ready */
            return QObject::tr("A07092 - Drive: moment of inertia estimator still not ready");
            break;
        case 7093:
            /* F07093 (A) - Drive: Test signal error */
            return QObject::tr("F07093 (A) - Drive: Test signal error");
            break;
        case 7094:
            /* A07094 - General parameter limit violation */
            return QObject::tr("A07094 - General parameter limit violation");
            break;
        case 7095:
            /* A07095 (N) - Drive: One Button Tuning activated */
            return QObject::tr("A07095 (N) - Drive: One Button Tuning activated");
            break;
        case 7097:
            /* F07097 (A) - Drive: Test signal error distance limiting */
            return QObject::tr("F07097 (A) - Drive: Test signal error distance limiting");
            break;
        case 7098:
            /* F07098 (A) - Drive: One Button Tuning configuration error */
            return QObject::tr("F07098 (A) - Drive: One Button Tuning configuration error");
            break;
        case 7100:
            /* F07100 - Drive: Sampling times cannot be reset */
            return QObject::tr("F07100 - Drive: Sampling times cannot be reset");
            break;
        case 7110:
            /* F07110 - Drive: Sampling times and basic clock cycle do not match */
            return QObject::tr("F07110 - Drive: Sampling times and basic clock cycle do not match");
            break;
        case 7140:
            /* A07140 - Drive: current controller sampling time for spindle does not match */
            return QObject::tr("A07140 - Drive: current controller sampling time for spindle does not match");
            break;
        case 7200:
            /* A07200 - Drive: Master control ON command present */
            return QObject::tr("A07200 - Drive: Master control ON command present");
            break;
        case 7220:
            /* F07220 (N, A) - Drive: Master control by PLC missing */
            return QObject::tr("F07220 (N, A) - Drive: Master control by PLC missing");
            break;
        case 7300:
            /* F07300 (A) - Drive: Line contactor feedback signal missing */
            return QObject::tr("F07300 (A) - Drive: Line contactor feedback signal missing");
            break;
        case 7311:
            /* F07311 - Bypass motor switch */
            return QObject::tr("F07311 - Bypass motor switch");
            break;
        case 7312:
            /* F07312 - Bypass Line Side Switch: */
            return QObject::tr("F07312 - Bypass Line Side Switch:");
            break;
        case 7320:
            /* F07320 - Drive: Automatic restart interrupted */
            return QObject::tr("F07320 - Drive: Automatic restart interrupted");
            break;
        case 7321:
            /* A07321 - Drive: Automatic restart active */
            return QObject::tr("A07321 - Drive: Automatic restart active");
            break;
        case 7329:
            /* A07329 (N) - Drive: kT estimator, kT(iq) characteristic or voltage compensation does not function */
            return QObject::tr("A07329 (N) - Drive: kT estimator, kT(iq) characteristic or voltage compensation does not function");
            break;
        case 7330:
            /* F07330 - Flying restart: Measured search current too low */
            return QObject::tr("F07330 - Flying restart: Measured search current too low");
            break;
        case 7331:
            /* F07331 - Flying restart: Function not supported */
            return QObject::tr("F07331 - Flying restart: Function not supported");
            break;
        case 7332:
            /* N07332 - Flying restart: maximum speed reduced */
            return QObject::tr("N07332 - Flying restart: maximum speed reduced");
            break;
        case 7333:
            /* F07333 - Closed-loop control function not supported */
            return QObject::tr("F07333 - Closed-loop control function not supported");
            break;
        case 7350:
            /* A07350 (F) - Drive: Measuring probe parameterized to a digital output */
            return QObject::tr("A07350 (F) - Drive: Measuring probe parameterized to a digital output");
            break;
        case 7351:
            /* A07351 (F) - Drive: Measuring probe parameterized to a digital output */
            return QObject::tr("A07351 (F) - Drive: Measuring probe parameterized to a digital output");
            break;
        case 7354:
            /* A07354 - Drive: cogging torque compensation not possible */
            return QObject::tr("A07354 - Drive: cogging torque compensation not possible");
            break;
        case 7355:
            /* F07355 (N, A) - Drive: cogging torque compensation error when learning */
            return QObject::tr("F07355 (N, A) - Drive: cogging torque compensation error when learning");
            break;
        case 7400:
            /* A07400 (N) - Drive: DC link voltage maximum controller active */
            return QObject::tr("A07400 (N) - Drive: DC link voltage maximum controller active");
            break;
        case 7401:
            /* A07401 (N) - Drive: DC link voltage maximum controller deactivated */
            return QObject::tr("A07401 (N) - Drive: DC link voltage maximum controller deactivated");
            break;
        case 7402:
            /* A07402 (N) - Drive: DC link voltage minimum controller active */
            return QObject::tr("A07402 (N) - Drive: DC link voltage minimum controller active");
            break;
        case 7403:
            /* F07403 (N, A) - Drive: Lower DC link voltage threshold reached */
            return QObject::tr("F07403 (N, A) - Drive: Lower DC link voltage threshold reached");
            break;
        case 7404:
            /* F07404 - Drive: Upper DC link voltage threshold reached */
            return QObject::tr("F07404 - Drive: Upper DC link voltage threshold reached");
            break;
        case 7405:
            /* F07405 (N, A) - Drive: Kinetic buffering minimum speed fallen below */
            return QObject::tr("F07405 (N, A) - Drive: Kinetic buffering minimum speed fallen below");
            break;
        case 7406:
            /* F07406 (N, A) - Drive: Kinetic buffering maximum time exceeded */
            return QObject::tr("F07406 (N, A) - Drive: Kinetic buffering maximum time exceeded");
            break;
        case 7407:
            /* F07407 - Drive: Vdc reduction not permissible */
            return QObject::tr("F07407 - Drive: Vdc reduction not permissible");
            break;
        case 7409:
            /* A07409 (N) - Drive: U/f control, current limiting controller active */
            return QObject::tr("A07409 (N) - Drive: U/f control, current limiting controller active");
            break;
        case 7410:
            /* F07410 - Drive: Current controller output limited */
            return QObject::tr("F07410 - Drive: Current controller output limited");
            break;
        case 7411:
            /* F07411 - Drive: Flux setpoint not reached when building up excitation */
            return QObject::tr("F07411 - Drive: Flux setpoint not reached when building up excitation");
            break;
        case 7412:
            /* F07412 - Drive: Commutation angle incorrect (motor model) */
            return QObject::tr("F07412 - Drive: Commutation angle incorrect (motor model)");
            break;
        case 7413:
            /* F07413 - Drive: Commutation angle incorrect (pole position identification) */
            return QObject::tr("F07413 - Drive: Commutation angle incorrect (pole position identification)");
            break;
        case 7414:
            /* F07414 (N, A) - Drive: Encoder serial number changed */
            return QObject::tr("F07414 (N, A) - Drive: Encoder serial number changed");
            break;
        case 7415:
            /* N07415 (F) - Drive: Angular commutation offset transfer running */
            return QObject::tr("N07415 (F) - Drive: Angular commutation offset transfer running");
            break;
        case 7416:
            /* A07416 - Drive: Flux controller configuration */
            return QObject::tr("A07416 - Drive: Flux controller configuration");
            break;
        case 7417:
            /* F07417 - Drive: Pulse technique not plausible (motor model) */
            return QObject::tr("F07417 - Drive: Pulse technique not plausible (motor model)");
            break;
        case 7419:
            /* F07419 - Drive: Current setpoint filter adaptation error */
            return QObject::tr("F07419 - Drive: Current setpoint filter adaptation error");
            break;
        case 7420:
            /* F07420 - Drive: Current setpoint filter natural frequency > Shannon frequency */
            return QObject::tr("F07420 - Drive: Current setpoint filter natural frequency > Shannon frequency");
            break;
        case 7421:
            /* F07421 - Drive: Speed filter natural frequency > Shannon frequency */
            return QObject::tr("F07421 - Drive: Speed filter natural frequency > Shannon frequency");
            break;
        case 7422:
            /* F07422 - Drive: Reference model natural frequency > Shannon frequency */
            return QObject::tr("F07422 - Drive: Reference model natural frequency > Shannon frequency");
            break;
        case 7423:
            /* F07423 - Drive: APC filter natural frequency > Shannon frequency */
            return QObject::tr("F07423 - Drive: APC filter natural frequency > Shannon frequency");
            break;
        case 7424:
            /* A07424 - Drive: Operating condition for APC not valid */
            return QObject::tr("A07424 - Drive: Operating condition for APC not valid");
            break;
        case 7425:
            /* F07425 - Drive: APC monitoring time for speed limit expired */
            return QObject::tr("F07425 - Drive: APC monitoring time for speed limit expired");
            break;
        case 7426:
            /* F07426 (A) - Technology controller actual value limited */
            return QObject::tr("F07426 (A) - Technology controller actual value limited");
            break;
        case 7428:
            /* A07428 (N) - Technology controller parameterizing error */
            return QObject::tr("A07428 (N) - Technology controller parameterizing error");
            break;
        case 7429:
            /* F07429 - Drive: DSC without encoder not possible */
            return QObject::tr("F07429 - Drive: DSC without encoder not possible");
            break;
        case 7430:
            /* F07430 - Drive: Changeover to open-loop torque controlled operation not possible */
            return QObject::tr("F07430 - Drive: Changeover to open-loop torque controlled operation not possible");
            break;
        case 7431:
            /* F07431 - Drive: Changeover to encoderless operation not possible */
            return QObject::tr("F07431 - Drive: Changeover to encoderless operation not possible");
            break;
        case 7432:
            /* F07432 - Drive: Motor without overvoltage protection */
            return QObject::tr("F07432 - Drive: Motor without overvoltage protection");
            break;
        case 7433:
            /* F07433 - Drive: Closed-loop control with encoder is not possible as the encoder has not been unparked */
            return QObject::tr("F07433 - Drive: Closed-loop control with encoder is not possible as the encoder has not been unparked");
            break;
        case 7434:
            /* F07434 - Drive: It is not possible to change the direction of rotation with the pulses enabled */
            return QObject::tr("F07434 - Drive: It is not possible to change the direction of rotation with the pulses enabled");
            break;
        case 7435:
            /* F07435 (N) - Drive: Setting the ramp-function generator for sensorless vector control */
            return QObject::tr("F07435 (N) - Drive: Setting the ramp-function generator for sensorless vector control");
            break;
        case 7439:
            /* F07439 - Drive: Function not supported */
            return QObject::tr("F07439 - Drive: Function not supported");
            break;
        case 7440:
            /* A07440 - EPOS: Jerk time is limited */
            return QObject::tr("A07440 - EPOS: Jerk time is limited");
            break;
        case 7441:
            /* A07441 - LR: Save the position offset of the absolute encoder adjustment */
            return QObject::tr("A07441 - LR: Save the position offset of the absolute encoder adjustment");
            break;
        case 7442:
            /* F07442 (A) - LR: Multiturn does not match the modulo range */
            return QObject::tr("F07442 (A) - LR: Multiturn does not match the modulo range");
            break;
        case 7443:
            /* F07443 (A) - LR: Reference point coordinate not in the permissible range */
            return QObject::tr("F07443 (A) - LR: Reference point coordinate not in the permissible range");
            break;
        case 7446:
            /* F07446 (A) - Load gear: Position tracking cannot be reset */
            return QObject::tr("F07446 (A) - Load gear: Position tracking cannot be reset");
            break;
        case 7447:
            /* F07447 - Load gear: Position tracking, maximum actual value exceeded */
            return QObject::tr("F07447 - Load gear: Position tracking, maximum actual value exceeded");
            break;
        case 7448:
            /* F07448 (A) - Load gear: Position tracking, linear axis has exceeded the maximum range */
            return QObject::tr("F07448 (A) - Load gear: Position tracking, linear axis has exceeded the maximum range");
            break;
        case 7449:
            /* F07449 (A) - Load gear: Position tracking actual position outside tolerance window */
            return QObject::tr("F07449 (A) - Load gear: Position tracking actual position outside tolerance window");
            break;
        case 7450:
            /* F07450 (A) - LR: Standstill monitoring has responded */
            return QObject::tr("F07450 (A) - LR: Standstill monitoring has responded");
            break;
        case 7451:
            /* F07451 (A) - LR: Position monitoring has responded */
            return QObject::tr("F07451 (A) - LR: Position monitoring has responded");
            break;
        case 7452:
            /* F07452 (A) - LR: Following error too high */
            return QObject::tr("F07452 (A) - LR: Following error too high");
            break;
        case 7453:
            /* F07453 - LR: Position actual value preprocessing error */
            return QObject::tr("F07453 - LR: Position actual value preprocessing error");
            break;
        case 7454:
            /* A07454 - LR: Position actual value preprocessing does not have a valid encoder */
            return QObject::tr("A07454 - LR: Position actual value preprocessing does not have a valid encoder");
            break;
        case 7455:
            /* A07455 - EPOS: Maximum velocity limited */
            return QObject::tr("A07455 - EPOS: Maximum velocity limited");
            break;
        case 7456:
            /* A07456 - EPOS: Setpoint velocity limited */
            return QObject::tr("A07456 - EPOS: Setpoint velocity limited");
            break;
        case 7457:
            /* A07457 - EPOS: Combination of input signals illegal */
            return QObject::tr("A07457 - EPOS: Combination of input signals illegal");
            break;
        case 7458:
            /* F07458 - EPOS: Reference cam not found */
            return QObject::tr("F07458 - EPOS: Reference cam not found");
            break;
        case 7459:
            /* F07459 - EPOS: No zero mark */
            return QObject::tr("F07459 - EPOS: No zero mark");
            break;
        case 7460:
            /* F07460 - EPOS: End of reference cam not found */
            return QObject::tr("F07460 - EPOS: End of reference cam not found");
            break;
        case 7461:
            /* A07461 - EPOS: Reference point not set */
            return QObject::tr("A07461 - EPOS: Reference point not set");
            break;
        case 7462:
            /* A07462 - EPOS: Selected traversing block number does not exist */
            return QObject::tr("A07462 - EPOS: Selected traversing block number does not exist");
            break;
        case 7463:
            /* A07463 (F) - EPOS: External block change not requested in the traversing block */
            return QObject::tr("A07463 (F) - EPOS: External block change not requested in the traversing block");
            break;
        case 7464:
            /* F07464 - EPOS: Traversing block is inconsistent */
            return QObject::tr("F07464 - EPOS: Traversing block is inconsistent");
            break;
        case 7465:
            /* A07465 - EPOS: Traversing block does not have a subsequent block */
            return QObject::tr("A07465 - EPOS: Traversing block does not have a subsequent block");
            break;
        case 7466:
            /* A07466 - EPOS: Traversing block number assigned a multiple number of times */
            return QObject::tr("A07466 - EPOS: Traversing block number assigned a multiple number of times");
            break;
        case 7467:
            /* A07467 - EPOS: Traversing block has illegal task parameters */
            return QObject::tr("A07467 - EPOS: Traversing block has illegal task parameters");
            break;
        case 7468:
            /* A07468 - EPOS: Traversing block jump destination does not exist */
            return QObject::tr("A07468 - EPOS: Traversing block jump destination does not exist");
            break;
        case 7469:
            /* A07469 - EPOS: Traversing block < target position < software limit switch minus */
            return QObject::tr("A07469 - EPOS: Traversing block < target position < software limit switch minus");
            break;
        case 7470:
            /* A07470 - EPOS: Traversing block> target position > software limit switch plus */
            return QObject::tr("A07470 - EPOS: Traversing block> target position > software limit switch plus");
            break;
        case 7471:
            /* A07471 - EPOS: Traversing block target position outside the modulo range */
            return QObject::tr("A07471 - EPOS: Traversing block target position outside the modulo range");
            break;
        case 7472:
            /* A07472 - EPOS: Traversing block ABS_POS/ABS_NEG not possible */
            return QObject::tr("A07472 - EPOS: Traversing block ABS_POS/ABS_NEG not possible");
            break;
        case 7473:
            /* A07473 (F) - EPOS: Beginning of traversing range reached */
            return QObject::tr("A07473 (F) - EPOS: Beginning of traversing range reached");
            break;
        case 7474:
            /* A07474 (F) - EPOS: End of traversing range reached */
            return QObject::tr("A07474 (F) - EPOS: End of traversing range reached");
            break;
        case 7475:
            /* F07475 (A) - EPOS: Target position < start of traversing range */
            return QObject::tr("F07475 (A) - EPOS: Target position < start of traversing range");
            break;
        case 7476:
            /* F07476 (A) - EPOS: Target position > end of the traversing range */
            return QObject::tr("F07476 (A) - EPOS: Target position > end of the traversing range");
            break;
        case 7477:
            /* A07477 (F) - EPOS: Target position < software limit switch minus */
            return QObject::tr("A07477 (F) - EPOS: Target position < software limit switch minus");
            break;
        case 7478:
            /* A07478 (F) - EPOS: Target position > software limit switch plus */
            return QObject::tr("A07478 (F) - EPOS: Target position > software limit switch plus");
            break;
        case 7479:
            /* A07479 - EPOS: Software limit switch minus reached */
            return QObject::tr("A07479 - EPOS: Software limit switch minus reached");
            break;
        case 7480:
            /* A07480 - EPOS: Software limit switch plus reached */
            return QObject::tr("A07480 - EPOS: Software limit switch plus reached");
            break;
        case 7481:
            /* F07481 (A) - EPOS: Axis position < software limit switch minus */
            return QObject::tr("F07481 (A) - EPOS: Axis position < software limit switch minus");
            break;
        case 7482:
            /* F07482 (A) - EPOS: Axis position > software limit switch plus */
            return QObject::tr("F07482 (A) - EPOS: Axis position > software limit switch plus");
            break;
        case 7483:
            /* A07483 - EPOS: Travel to fixed stop clamping torque not reached */
            return QObject::tr("A07483 - EPOS: Travel to fixed stop clamping torque not reached");
            break;
        case 7484:
            /* F07484 - EPOS: Fixed stop outside the monitoring window */
            return QObject::tr("F07484 - EPOS: Fixed stop outside the monitoring window");
            break;
        case 7485:
            /* F07485 (A) - EPOS: Fixed stop not reached */
            return QObject::tr("F07485 (A) - EPOS: Fixed stop not reached");
            break;
        case 7486:
            /* A07486 - EPOS: Intermediate stop missing */
            return QObject::tr("A07486 - EPOS: Intermediate stop missing");
            break;
        case 7487:
            /* A07487 - EPOS: Reject traversing task missing */
            return QObject::tr("A07487 - EPOS: Reject traversing task missing");
            break;
        case 7488:
            /* F07488 - EPOS: Relative positioning not possible */
            return QObject::tr("F07488 - EPOS: Relative positioning not possible");
            break;
        case 7489:
            /* A07489 - EPOS: Reference point correction outside the window */
            return QObject::tr("A07489 - EPOS: Reference point correction outside the window");
            break;
        case 7490:
            /* F07490 (N) - EPOS: Enable signal withdrawn while traversing */
            return QObject::tr("F07490 (N) - EPOS: Enable signal withdrawn while traversing");
            break;
        case 7491:
            /* F07491 (A) - EPOS: STOP cam minus reached */
            return QObject::tr("F07491 (A) - EPOS: STOP cam minus reached");
            break;
        case 7492:
            /* F07492 (A) - EPOS: STOP cam plus reached */
            return QObject::tr("F07492 (A) - EPOS: STOP cam plus reached");
            break;
        case 7493:
            /* F07493 - LR: Overflow of the value range for position actual value */
            return QObject::tr("F07493 - LR: Overflow of the value range for position actual value");
            break;
        case 7494:
            /* F07494 - LR: Drive Data Set changeover in operation */
            return QObject::tr("F07494 - LR: Drive Data Set changeover in operation");
            break;
        case 7495:
            /* A07495 (F, N) - LR: Reference function interrupted */
            return QObject::tr("A07495 (F, N) - LR: Reference function interrupted");
            break;
        case 7496:
            /* A07496 - EPOS: Enable not possible */
            return QObject::tr("A07496 - EPOS: Enable not possible");
            break;
        case 7497:
            /* A07497 (N) - LR: Position setting value activated */
            return QObject::tr("A07497 (N) - LR: Position setting value activated");
            break;
        case 7498:
            /* A07498 (F) - LR: Measuring probe evaluation not possible */
            return QObject::tr("A07498 (F) - LR: Measuring probe evaluation not possible");
            break;
        case 7499:
            /* F07499 (A) - EPOS: Reversing cam approached with the incorrect traversing direction */
            return QObject::tr("F07499 (A) - EPOS: Reversing cam approached with the incorrect traversing direction");
            break;
        case 7500:
            /* F07500 - Drive: Power unit data set PDS not configured */
            return QObject::tr("F07500 - Drive: Power unit data set PDS not configured");
            break;
        case 7501:
            /* F07501 - Drive: Motor Data Set MDS not configured */
            return QObject::tr("F07501 - Drive: Motor Data Set MDS not configured");
            break;
        case 7502:
            /* F07502 - Drive: Encoder Data Set EDS not configured */
            return QObject::tr("F07502 - Drive: Encoder Data Set EDS not configured");
            break;
        case 7503:
            /* F07503 - EPOS: STOP cam approached with the incorrect traversing direction */
            return QObject::tr("F07503 - EPOS: STOP cam approached with the incorrect traversing direction");
            break;
        case 7504:
            /* A07504 - Drive: Motor data set is not assigned to a drive data set */
            return QObject::tr("A07504 - Drive: Motor data set is not assigned to a drive data set");
            break;
        case 7505:
            /* A07505 - EPOS: Task fixed stop not possible in the U/f/SLVC mode */
            return QObject::tr("A07505 - EPOS: Task fixed stop not possible in the U/f/SLVC mode");
            break;
        case 7506:
            /* A07506 - EPOS: check BICO interconnection between EPOS and position controller */
            return QObject::tr("A07506 - EPOS: check BICO interconnection between EPOS and position controller");
            break;
        case 7507:
            /* A07507 - EPOS: reference point cannot be set */
            return QObject::tr("A07507 - EPOS: reference point cannot be set");
            break;
        case 7509:
            /* F07509 - Drive: Component assignment missing */
            return QObject::tr("F07509 - Drive: Component assignment missing");
            break;
        case 7510:
            /* F07510 - Drive: Identical encoder in the drive data set */
            return QObject::tr("F07510 - Drive: Identical encoder in the drive data set");
            break;
        case 7511:
            /* F07511 - Drive: Encoder used a multiple number of times */
            return QObject::tr("F07511 - Drive: Encoder used a multiple number of times");
            break;
        case 7512:
            /* F07512 - Drive: Encoder data set changeover cannot be parameterized */
            return QObject::tr("F07512 - Drive: Encoder data set changeover cannot be parameterized");
            break;
        case 7514:
            /* A07514 (N) - Drive: Data structure does not correspond to the interface module */
            return QObject::tr("A07514 (N) - Drive: Data structure does not correspond to the interface module");
            break;
        case 7515:
            /* F07515 - Drive: Power unit and motor incorrectly connected */
            return QObject::tr("F07515 - Drive: Power unit and motor incorrectly connected");
            break;
        case 7516:
            /* F07516 - Drive: Re-commission the data set */
            return QObject::tr("F07516 - Drive: Re-commission the data set");
            break;
        case 7517:
            /* F07517 - Drive: Encoder data set changeover incorrectly parameterized */
            return QObject::tr("F07517 - Drive: Encoder data set changeover incorrectly parameterized");
            break;
        case 7518:
            /* F07518 - Drive: Motor data set changeover incorrectly parameterized */
            return QObject::tr("F07518 - Drive: Motor data set changeover incorrectly parameterized");
            break;
        case 7519:
            /* A07519 - Drive: Motor changeover incorrectly parameterized */
            return QObject::tr("A07519 - Drive: Motor changeover incorrectly parameterized");
            break;
        case 7520:
            /* A07520 - Drive: Motor cannot be changed over */
            return QObject::tr("A07520 - Drive: Motor cannot be changed over");
            break;
        case 7530:
            /* A07530 - Drive: Drive Data Set DDS not present */
            return QObject::tr("A07530 - Drive: Drive Data Set DDS not present");
            break;
        case 7531:
            /* A07531 - Drive: Command Data Set CDS not present */
            return QObject::tr("A07531 - Drive: Command Data Set CDS not present");
            break;
        case 7541:
            /* A07541 - Drive: Data set changeover not possible */
            return QObject::tr("A07541 - Drive: Data set changeover not possible");
            break;
        case 7550:
            /* A07550 (F, N) - Drive: Not possible to reset encoder parameters */
            return QObject::tr("A07550 (F, N) - Drive: Not possible to reset encoder parameters");
            break;
        case 7551:
            /* F07551 - Drive encoder: No commutation angle information */
            return QObject::tr("F07551 - Drive encoder: No commutation angle information");
            break;
        case 7552:
            /* F07552 (A) - Drive encoder: Encoder configuration not supported */
            return QObject::tr("F07552 (A) - Drive encoder: Encoder configuration not supported");
            break;
        case 7553:
            /* F07553 (A) - Drive encoder: Sensor Module configuration not supported */
            return QObject::tr("F07553 (A) - Drive encoder: Sensor Module configuration not supported");
            break;
        case 7555:
            /* F07555 (A) - Drive encoder: Configuration position tracking */
            return QObject::tr("F07555 (A) - Drive encoder: Configuration position tracking");
            break;
        case 7556:
            /* F07556 - Measuring gear: Position tracking, maximum actual value exceeded */
            return QObject::tr("F07556 - Measuring gear: Position tracking, maximum actual value exceeded");
            break;
        case 7557:
            /* A07557 (F) - Encoder 1: Reference point coordinate not in the permissible range */
            return QObject::tr("A07557 (F) - Encoder 1: Reference point coordinate not in the permissible range");
            break;
        case 7558:
            /* A07558 (F) - Encoder 2: Reference point coordinate not in the permissible range */
            return QObject::tr("A07558 (F) - Encoder 2: Reference point coordinate not in the permissible range");
            break;
        case 7559:
            /* A07559 (F) - Encoder 3: Reference point coordinate not in the permissible range */
            return QObject::tr("A07559 (F) - Encoder 3: Reference point coordinate not in the permissible range");
            break;
        case 7560:
            /* F07560 - Drive encoder: Number of pulses is not to the power of two */
            return QObject::tr("F07560 - Drive encoder: Number of pulses is not to the power of two");
            break;
        case 7561:
            /* F07561 - Drive encoder: Number of multiturn pulses is not to the power of two */
            return QObject::tr("F07561 - Drive encoder: Number of multiturn pulses is not to the power of two");
            break;
        case 7562:
            /* F07562 (A) - Drive, encoder: Position tracking, incremental encoder not possible */
            return QObject::tr("F07562 (A) - Drive, encoder: Position tracking, incremental encoder not possible");
            break;
        case 7563:
            /* F07563 (A) - Drive encoder: XIST1_ERW configuration incorrect */
            return QObject::tr("F07563 (A) - Drive encoder: XIST1_ERW configuration incorrect");
            break;
        case 7565:
            /* A07565 (F, N) - Drive: Encoder error in PROFIdrive encoder interface 1 */
            return QObject::tr("A07565 (F, N) - Drive: Encoder error in PROFIdrive encoder interface 1");
            break;
        case 7566:
            /* A07566 (F, N) - Drive: Encoder error in PROFIdrive encoder interface 2 */
            return QObject::tr("A07566 (F, N) - Drive: Encoder error in PROFIdrive encoder interface 2");
            break;
        case 7567:
            /* A07567 (F, N) - Drive: Encoder error in PROFIdrive encoder interface 3 */
            return QObject::tr("A07567 (F, N) - Drive: Encoder error in PROFIdrive encoder interface 3");
            break;
        case 7569:
            /* A07569 (F) - Enc identification active */
            return QObject::tr("A07569 (F) - Enc identification active");
            break;
        case 7570:
            /* N07570 (F) - Encoder identification data transfer running */
            return QObject::tr("N07570 (F) - Encoder identification data transfer running");
            break;
        case 7575:
            /* F07575 - Drive: Motor encoder not ready */
            return QObject::tr("F07575 - Drive: Motor encoder not ready");
            break;
        case 7576:
            /* A07576 - Drive: Encoderless operation due to a fault active */
            return QObject::tr("A07576 - Drive: Encoderless operation due to a fault active");
            break;
        case 7577:
            /* A07577 (F) - Encoder 1: Measuring probe evaluation not possible */
            return QObject::tr("A07577 (F) - Encoder 1: Measuring probe evaluation not possible");
            break;
        case 7578:
            /* A07578 (F) - Encoder 2: Measuring probe evaluation not possible */
            return QObject::tr("A07578 (F) - Encoder 2: Measuring probe evaluation not possible");
            break;
        case 7579:
            /* A07579 (F) - Encoder 3: Measuring probe evaluation not possible */
            return QObject::tr("A07579 (F) - Encoder 3: Measuring probe evaluation not possible");
            break;
        case 7580:
            /* A07580 (F, N) - Drive: No Sensor Module with matching component number */
            return QObject::tr("A07580 (F, N) - Drive: No Sensor Module with matching component number");
            break;
        case 7581:
            /* A07581 (F) - Encoder 1: Position actual value preprocessing error */
            return QObject::tr("A07581 (F) - Encoder 1: Position actual value preprocessing error");
            break;
        case 7582:
            /* A07582 (F) - Encoder 2: Position actual value preprocessing error */
            return QObject::tr("A07582 (F) - Encoder 2: Position actual value preprocessing error");
            break;
        case 7583:
            /* A07583 (F) - Encoder 3: Position actual value preprocessing error */
            return QObject::tr("A07583 (F) - Encoder 3: Position actual value preprocessing error");
            break;
        case 7584:
            /* A07584 - Encoder 1: Position setting value activated */
            return QObject::tr("A07584 - Encoder 1: Position setting value activated");
            break;
        case 7585:
            /* A07585 - Encoder 2: Position setting value activated */
            return QObject::tr("A07585 - Encoder 2: Position setting value activated");
            break;
        case 7586:
            /* A07586 - Encoder 3: Position setting value activated */
            return QObject::tr("A07586 - Encoder 3: Position setting value activated");
            break;
        case 7587:
            /* A07587 - Encoder 1: Position actual value preprocessing does not have a valid encoder */
            return QObject::tr("A07587 - Encoder 1: Position actual value preprocessing does not have a valid encoder");
            break;
        case 7588:
            /* A07588 - Encoder 2: Position actual value preprocessing does not have a valid encoder */
            return QObject::tr("A07588 - Encoder 2: Position actual value preprocessing does not have a valid encoder");
            break;
        case 7589:
            /* A07589 - Encoder 3: Position actual value preprocessing does not have a valid encoder */
            return QObject::tr("A07589 - Encoder 3: Position actual value preprocessing does not have a valid encoder");
            break;
        case 7590:
            /* A07590 (F) - Encoder 1: Drive Data Set changeover in operation */
            return QObject::tr("A07590 (F) - Encoder 1: Drive Data Set changeover in operation");
            break;
        case 7591:
            /* A07591 (F) - Encoder 2: Drive Data Set changeover in operation */
            return QObject::tr("A07591 (F) - Encoder 2: Drive Data Set changeover in operation");
            break;
        case 7592:
            /* A07592 (F) - Encoder 3: Drive Data Set changeover in operation */
            return QObject::tr("A07592 (F) - Encoder 3: Drive Data Set changeover in operation");
            break;
        case 7593:
            /* A07593 (F, N) - Encoder 1: Value range for position actual value exceeded */
            return QObject::tr("A07593 (F, N) - Encoder 1: Value range for position actual value exceeded");
            break;
        case 7594:
            /* A07594 (F, N) - Encoder 2: Value range for position actual value exceeded */
            return QObject::tr("A07594 (F, N) - Encoder 2: Value range for position actual value exceeded");
            break;
        case 7595:
            /* A07595 (F, N) - Encoder 3: Value range for position actual value exceeded */
            return QObject::tr("A07595 (F, N) - Encoder 3: Value range for position actual value exceeded");
            break;
        case 7596:
            /* A07596 (F, N) - Encoder 1: Reference function interrupted */
            return QObject::tr("A07596 (F, N) - Encoder 1: Reference function interrupted");
            break;
        case 7597:
            /* A07597 (F, N) - Encoder 2: Reference function interrupted */
            return QObject::tr("A07597 (F, N) - Encoder 2: Reference function interrupted");
            break;
        case 7598:
            /* A07598 (F, N) - Encoder 3: Reference function interrupted */
            return QObject::tr("A07598 (F, N) - Encoder 3: Reference function interrupted");
            break;
        case 7599:
            /* F07599 (A) - Encoder 1: Adjustment not possible */
            return QObject::tr("F07599 (A) - Encoder 1: Adjustment not possible");
            break;
        case 7600:
            /* F07600 (A) - Encoder 2: Adjustment not possible */
            return QObject::tr("F07600 (A) - Encoder 2: Adjustment not possible");
            break;
        case 7601:
            /* F07601 (A) - Encoder 3: Adjustment not possible */
            return QObject::tr("F07601 (A) - Encoder 3: Adjustment not possible");
            break;
        case 7750:
            /* F07750 - Drive: Parameter invalid */
            return QObject::tr("F07750 - Drive: Parameter invalid");
            break;
        case 7751:
            /* F07751 - Drive: valve does not respond */
            return QObject::tr("F07751 - Drive: valve does not respond");
            break;
        case 7752:
            /* F07752 (A) - Drive: Piston position not possible */
            return QObject::tr("F07752 (A) - Drive: Piston position not possible");
            break;
        case 7753:
            /* F07753 (N, A) - Drive: No valid pressure actual value available */
            return QObject::tr("F07753 (N, A) - Drive: No valid pressure actual value available");
            break;
        case 7754:
            /* F07754 - Drive: Incorrect shutoff valve configuration */
            return QObject::tr("F07754 - Drive: Incorrect shutoff valve configuration");
            break;
        case 7755:
            /* F07755 (N, A) - Drive: travel to fixed end stop without force controller */
            return QObject::tr("F07755 (N, A) - Drive: travel to fixed end stop without force controller");
            break;
        case 7756:
            /* F07756 - Drive: Filter natural frequency > Shannon frequency */
            return QObject::tr("F07756 - Drive: Filter natural frequency > Shannon frequency");
            break;
        case 7800:
            /* F07800 - Drive: No power unit present */
            return QObject::tr("F07800 - Drive: No power unit present");
            break;
        case 7801:
            /* F07801 - Drive: Motor overcurrent */
            return QObject::tr("F07801 - Drive: Motor overcurrent");
            break;
        case 7802:
            /* F07802 - Drive: Infeed or power unit not ready */
            return QObject::tr("F07802 - Drive: Infeed or power unit not ready");
            break;
        case 7805:
            /* A07805 (N) - Drive: Power unit overload I2t */
            return QObject::tr("A07805 (N) - Drive: Power unit overload I2t");
            break;
        case 7807:
            /* F07807 - Drive: Short-circuit/ground fault detected */
            return QObject::tr("F07807 - Drive: Short-circuit/ground fault detected");
            break;
        case 7808:
            /* F07808 (A) - HF Damping Module: damping not ready */
            return QObject::tr("F07808 (A) - HF Damping Module: damping not ready");
            break;
        case 7810:
            /* F07810 - Drive: Power unit EEPROM without rated data */
            return QObject::tr("F07810 - Drive: Power unit EEPROM without rated data");
            break;
        case 7815:
            /* F07815 - Drive: Power unit has been changed */
            return QObject::tr("F07815 - Drive: Power unit has been changed");
            break;
        case 7820:
            /* A07820 - Drive: Temperature sensor not connected */
            return QObject::tr("A07820 - Drive: Temperature sensor not connected");
            break;
        case 7821:
            /* A07821 - Monitoring underspeed threshold fallen below alarm */
            return QObject::tr("A07821 - Monitoring underspeed threshold fallen below alarm");
            break;
        case 7822:
            /* F07822 (N) - Monitoring underspeed threshold fallen below fault */
            return QObject::tr("F07822 (N) - Monitoring underspeed threshold fallen below fault");
            break;
        case 7823:
            /* A07823 - I2t monitoring alarm threshold exceeded */
            return QObject::tr("A07823 - I2t monitoring alarm threshold exceeded");
            break;
        case 7824:
            /* F07824 - I2t monitoring fault threshold exceeded */
            return QObject::tr("F07824 - I2t monitoring fault threshold exceeded");
            break;
        case 7825:
            /* A07825 (N) - Drive: Simulation mode activated */
            return QObject::tr("A07825 (N) - Drive: Simulation mode activated");
            break;
        case 7826:
            /* F07826 - Drive: DC link voltage for simulation operation too high */
            return QObject::tr("F07826 - Drive: DC link voltage for simulation operation too high");
            break;
        case 7840:
            /* F07840 - Drive: Infeed operation missing */
            return QObject::tr("F07840 - Drive: Infeed operation missing");
            break;
        case 7841:
            /* F07841 (A) - Drive: Infeed operation withdrawn */
            return QObject::tr("F07841 (A) - Drive: Infeed operation withdrawn");
            break;
        case 7850:
            /* A07850 (F) - External alarm 1 */
            return QObject::tr("A07850 (F) - External alarm 1");
            break;
        case 7851:
            /* A07851 (F) - External alarm 2 */
            return QObject::tr("A07851 (F) - External alarm 2");
            break;
        case 7852:
            /* A07852 (F) - External alarm 3 */
            return QObject::tr("A07852 (F) - External alarm 3");
            break;
        case 7860:
            /* F07860 (A) - External fault 1 */
            return QObject::tr("F07860 (A) - External fault 1");
            break;
        case 7861:
            /* F07861 (A) - External fault 2 */
            return QObject::tr("F07861 (A) - External fault 2");
            break;
        case 7862:
            /* F07862 (A) - External fault 3 */
            return QObject::tr("F07862 (A) - External fault 3");
            break;
        case 7890:
            /* F07890 - Internal voltage protection / internal armature short-circuit with STO active */
            return QObject::tr("F07890 - Internal voltage protection / internal armature short-circuit with STO active");
            break;
        case 7898:
            /* F07898 - Drive: flying restart unsuccessful due to excessively low flux */
            return QObject::tr("F07898 - Drive: flying restart unsuccessful due to excessively low flux");
            break;
        case 7899:
            /* A07899 (N) - Drive: Stall monitoring not possible */
            return QObject::tr("A07899 (N) - Drive: Stall monitoring not possible");
            break;
        case 7900:
            /* F07900 (N, A) - Drive: Motor blocked */
            return QObject::tr("F07900 (N, A) - Drive: Motor blocked");
            break;
        case 7901:
            /* F07901 - Drive: Motor overspeed */
            return QObject::tr("F07901 - Drive: Motor overspeed");
            break;
        case 7902:
            /* F07902 (N, A) - Drive: Motor stalled */
            return QObject::tr("F07902 (N, A) - Drive: Motor stalled");
            break;
        case 7903:
            /* A07903 - Drive: Motor speed deviation */
            return QObject::tr("A07903 - Drive: Motor speed deviation");
            break;
        case 7904:
            /* F07904 (N, A) - External armature short-circuit: Contactor feedback signal "Closed" missing */
            return QObject::tr("F07904 (N, A) - External armature short-circuit: Contactor feedback signal \"Closed\" missing");
            break;
        case 7905:
            /* F07905 (N, A) - External armature short-circuit: Contactor feedback signal "Open" missing */
            return QObject::tr("F07905 (N, A) - External armature short-circuit: Contactor feedback signal \"Open\" missing");
            break;
        case 7906:
            /* F07906 - Armature short-circuit / internal voltage protection: Parameterization error */
            return QObject::tr("F07906 - Armature short-circuit / internal voltage protection: Parameterization error");
            break;
        case 7907:
            /* F07907 - Internal armature short-circuit: Motor terminals are not at zero potential after pulse suppression */
            return QObject::tr("F07907 - Internal armature short-circuit: Motor terminals are not at zero potential after pulse suppression");
            break;
        case 7908:
            /* A07908 - Internal armature short-circuit active */
            return QObject::tr("A07908 - Internal armature short-circuit active");
            break;
        case 7909:
            /* F07909 - Internal voltage protection: Deactivation only effective after POWER ON */
            return QObject::tr("F07909 - Internal voltage protection: Deactivation only effective after POWER ON");
            break;
        case 7910:
            /* A07910 (N) - Drive: Motor overtemperature */
            return QObject::tr("A07910 (N) - Drive: Motor overtemperature");
            break;
        case 7913:
            /* F07913 - Excitation current outside the tolerance range */
            return QObject::tr("F07913 - Excitation current outside the tolerance range");
            break;
        case 7914:
            /* F07914 - Flux out of tolerance */
            return QObject::tr("F07914 - Flux out of tolerance");
            break;
        case 7918:
            /* A07918 (N) - Three-phase setpoint generator operation selected/active */
            return QObject::tr("A07918 (N) - Three-phase setpoint generator operation selected/active");
            break;
        case 7920:
            /* A07920 - Drive: Torque/speed too low */
            return QObject::tr("A07920 - Drive: Torque/speed too low");
            break;
        case 7921:
            /* A07921 - Drive: Torque/speed too high */
            return QObject::tr("A07921 - Drive: Torque/speed too high");
            break;
        case 7922:
            /* A07922 - Drive: Torque/speed out of tolerance */
            return QObject::tr("A07922 - Drive: Torque/speed out of tolerance");
            break;
        case 7923:
            /* F07923 - Drive: Torque/speed too low */
            return QObject::tr("F07923 - Drive: Torque/speed too low");
            break;
        case 7924:
            /* F07924 - Drive: Torque/speed too high */
            return QObject::tr("F07924 - Drive: Torque/speed too high");
            break;
        case 7925:
            /* F07925 - Drive: Torque/speed out of tolerance */
            return QObject::tr("F07925 - Drive: Torque/speed out of tolerance");
            break;
        case 7926:
            /* A07926 - Drive: Envelope curve parameter invalid */
            return QObject::tr("A07926 - Drive: Envelope curve parameter invalid");
            break;
        case 7927:
            /* A07927 - DC braking active */
            return QObject::tr("A07927 - DC braking active");
            break;
        case 7928:
            /* F07928 - Internal voltage protection initiated */
            return QObject::tr("F07928 - Internal voltage protection initiated");
            break;
        case 7930:
            /* F07930 - Drive: Brake control error */
            return QObject::tr("F07930 - Drive: Brake control error");
            break;
        case 7931:
            /* A07931 (F, N) - Brake does not open */
            return QObject::tr("A07931 (F, N) - Brake does not open");
            break;
        case 7932:
            /* A07932 - Brake does not close */
            return QObject::tr("A07932 - Brake does not close");
            break;
        case 7934:
            /* F07934 (N) - Drive: S120 Combi motor holding brake configuration */
            return QObject::tr("F07934 (N) - Drive: S120 Combi motor holding brake configuration");
            break;
        case 7935:
            /* F07935 (N) - Drive: Incorrect motor holding brake configuration */
            return QObject::tr("F07935 (N) - Drive: Incorrect motor holding brake configuration");
            break;
        case 7937:
            /* F07937 (N) - Drive: Speed deviation between motor model and external speed */
            return QObject::tr("F07937 (N) - Drive: Speed deviation between motor model and external speed");
            break;
        case 7940:
            /* F07940 - Sync-line-drive: Synchronizing error */
            return QObject::tr("F07940 - Sync-line-drive: Synchronizing error");
            break;
        case 7941:
            /* A07941 - Sync-line-drive: Target frequency not permissible */
            return QObject::tr("A07941 - Sync-line-drive: Target frequency not permissible");
            break;
        case 7942:
            /* A07942 - Sync-line-drive: Setpoint frequency is completely different than the target frequency */
            return QObject::tr("A07942 - Sync-line-drive: Setpoint frequency is completely different than the target frequency");
            break;
        case 7943:
            /* A07943 - Sync-line-drive: Synchronization not permitted */
            return QObject::tr("A07943 - Sync-line-drive: Synchronization not permitted");
            break;
        case 7950:
            /* F07950 (A) - Drive: Incorrect motor parameter */
            return QObject::tr("F07950 (A) - Drive: Incorrect motor parameter");
            break;
        case 7955:
            /* F07955 - Drive: Motor has been changed */
            return QObject::tr("F07955 - Drive: Motor has been changed");
            break;
        case 7956:
            /* F07956 (A) - Drive: Motor code does not match the list (catalog) motor */
            return QObject::tr("F07956 (A) - Drive: Motor code does not match the list (catalog) motor");
            break;
        case 7960:
            /* A07960 - Drive: Incorrect friction characteristic */
            return QObject::tr("A07960 - Drive: Incorrect friction characteristic");
            break;
        case 7961:
            /* A07961 - Drive: Record friction characteristic activated */
            return QObject::tr("A07961 - Drive: Record friction characteristic activated");
            break;
        case 7963:
            /* F07963 - Drive: Record friction characteristic canceled */
            return QObject::tr("F07963 - Drive: Record friction characteristic canceled");
            break;
        case 7965:
            /* A07965 (N) - Drive: Save required */
            return QObject::tr("A07965 (N) - Drive: Save required");
            break;
        case 7966:
            /* F07966 - Drive: Check the commutation angle */
            return QObject::tr("F07966 - Drive: Check the commutation angle");
            break;
        case 7967:
            /* F07967 - Drive: Automatic encoder adjustment/pole position identification incorrect */
            return QObject::tr("F07967 - Drive: Automatic encoder adjustment/pole position identification incorrect");
            break;
        case 7968:
            /* F07968 - Drive: Lq-Ld measurement incorrect */
            return QObject::tr("F07968 - Drive: Lq-Ld measurement incorrect");
            break;
        case 7969:
            /* F07969 - Drive: Incorrect pole position identification */
            return QObject::tr("F07969 - Drive: Incorrect pole position identification");
            break;
        case 7970:
            /* F07970 - Drive: Automatic encoder adjustment incorrect */
            return QObject::tr("F07970 - Drive: Automatic encoder adjustment incorrect");
            break;
        case 7971:
            /* A07971 (N) - Drive: Angular commutation offset determination activated */
            return QObject::tr("A07971 (N) - Drive: Angular commutation offset determination activated");
            break;
        case 7975:
            /* A07975 (N) - Drive: Travel to the zero mark - setpoint input expected */
            return QObject::tr("A07975 (N) - Drive: Travel to the zero mark - setpoint input expected");
            break;
        case 7976:
            /* A07976 - Drive: Fine encoder calibration activated */
            return QObject::tr("A07976 - Drive: Fine encoder calibration activated");
            break;
        case 7978:
            /* A07978 (N) - Drive: activated ESM mode waits for the end of motor identification */
            return QObject::tr("A07978 (N) - Drive: activated ESM mode waits for the end of motor identification");
            break;
        case 7979:
            /* A07979 (F, N) - Drive: pole position identification calibration required */
            return QObject::tr("A07979 (F, N) - Drive: pole position identification calibration required");
            break;
        case 7980:
            /* A07980 - Drive: Rotating measurement activated */
            return QObject::tr("A07980 - Drive: Rotating measurement activated");
            break;
        case 7981:
            /* A07981 - Drive: Enable signals for the rotating measurement missing */
            return QObject::tr("A07981 - Drive: Enable signals for the rotating measurement missing");
            break;
        case 7982:
            /* F07982 - Drive: Rotating measurement encoder test */
            return QObject::tr("F07982 - Drive: Rotating measurement encoder test");
            break;
        case 7983:
            /* F07983 - Drive: Rotating measurement saturation characteristic */
            return QObject::tr("F07983 - Drive: Rotating measurement saturation characteristic");
            break;
        case 7984:
            /* F07984 - Drive: Speed controller optimization, moment of inertia */
            return QObject::tr("F07984 - Drive: Speed controller optimization, moment of inertia");
            break;
        case 7985:
            /* F07985 - Drive: Speed controller optimization (oscillation test) */
            return QObject::tr("F07985 - Drive: Speed controller optimization (oscillation test)");
            break;
        case 7986:
            /* F07986 - Drive: Rotating measurement ramp-function generator */
            return QObject::tr("F07986 - Drive: Rotating measurement ramp-function generator");
            break;
        case 7987:
            /* A07987 - Drive: Rotating measurement, no encoder available */
            return QObject::tr("A07987 - Drive: Rotating measurement, no encoder available");
            break;
        case 7988:
            /* F07988 - Drive: Rotating measurement, no configuration selected */
            return QObject::tr("F07988 - Drive: Rotating measurement, no configuration selected");
            break;
        case 7989:
            /* F07989 - Drive: Rotating measurement leakage inductance (q-axis) */
            return QObject::tr("F07989 - Drive: Rotating measurement leakage inductance (q-axis)");
            break;
        case 7990:
            /* F07990 - Drive: Identification incorrect */
            return QObject::tr("F07990 - Drive: Identification incorrect");
            break;
        case 7991:
            /* A07991 (N) - Drive: Data identification activated */
            return QObject::tr("A07991 (N) - Drive: Data identification activated");
            break;
        case 7993:
            /* F07993 - Drive: Incorrect direction of rotation of the field or encoder actual value inversion */
            return QObject::tr("F07993 - Drive: Incorrect direction of rotation of the field or encoder actual value inversion");
            break;
        case 7994:
            /* A07994 (F, N) - Drive: motor data identification not performed */
            return QObject::tr("A07994 (F, N) - Drive: motor data identification not performed");
            break;
        case 7995:
            /* F07995 - Drive: Pole position identification not successful */
            return QObject::tr("F07995 - Drive: Pole position identification not successful");
            break;
        case 7996:
            /* F07996 - Drive: Pole position identification routine not carried out */
            return QObject::tr("F07996 - Drive: Pole position identification routine not carried out");
            break;
        case 7998:
            /* A07998 - Drive: Motor data identification active on another drive */
            return QObject::tr("A07998 - Drive: Motor data identification active on another drive");
            break;
        case 7999:
            /* A07999 - Drive: Motor data identification cannot be activated */
            return QObject::tr("A07999 - Drive: Motor data identification cannot be activated");
            break;
        case 8000:
            /* F08000 (N, A) - TB: +/-15 V power supply faulted */
            return QObject::tr("F08000 (N, A) - TB: +/-15 V power supply faulted");
            break;
        case 8010:
            /* F08010 (N, A) - TB: Analog-digital converter */
            return QObject::tr("F08010 (N, A) - TB: Analog-digital converter");
            break;
        case 8500:
            /* F08500 (A) - COMM BOARD: Monitoring time configuration expired */
            return QObject::tr("F08500 (A) - COMM BOARD: Monitoring time configuration expired");
            break;
        case 8501:
            /* F08501 (N, A) - PN/COMM BOARD: Setpoint timeout */
            return QObject::tr("F08501 (N, A) - PN/COMM BOARD: Setpoint timeout");
            break;
        case 8502:
            /* F08502 (A) - PN/COMM BOARD: Monitoring time sign-of-life expired */
            return QObject::tr("F08502 (A) - PN/COMM BOARD: Monitoring time sign-of-life expired");
            break;
        case 8504:
            /* A08504 (F) - PN/COMM BOARD: Internal cyclic data transfer error */
            return QObject::tr("A08504 (F) - PN/COMM BOARD: Internal cyclic data transfer error");
            break;
        case 8510:
            /* F08510 (A) - PN/COMM BOARD: Send configuration data invalid */
            return QObject::tr("F08510 (A) - PN/COMM BOARD: Send configuration data invalid");
            break;
        case 8511:
            /* A08511 (F) - PN/COMM BOARD: Receive configuration data invalid */
            return QObject::tr("A08511 (F) - PN/COMM BOARD: Receive configuration data invalid");
            break;
        case 8520:
            /* A08520 (F) - PN/COMM BOARD: Non-cyclic channel error */
            return QObject::tr("A08520 (F) - PN/COMM BOARD: Non-cyclic channel error");
            break;
        case 8526:
            /* A08526 (F) - PN/COMM BOARD: No cyclic connection */
            return QObject::tr("A08526 (F) - PN/COMM BOARD: No cyclic connection");
            break;
        case 8530:
            /* A08530 (F) - PN/COMM BOARD: Message channel error */
            return QObject::tr("A08530 (F) - PN/COMM BOARD: Message channel error");
            break;
        case 8531:
            /* A08531 (F) - CBE20 POWER ON required */
            return QObject::tr("A08531 (F) - CBE20 POWER ON required");
            break;
        case 8550:
            /* A08550 - PZD Interface Hardware assignment error */
            return QObject::tr("A08550 - PZD Interface Hardware assignment error");
            break;
        case 8555:
            /* A08555 - Modbus TCP: commissioning error */
            return QObject::tr("A08555 - Modbus TCP: commissioning error");
            break;
        case 8560:
            /* A08560 - IE: Syntax error in configuration file */
            return QObject::tr("A08560 - IE: Syntax error in configuration file");
            break;
        case 8561:
            /* A08561 - IE: Consistency error affecting adjustable parameters */
            return QObject::tr("A08561 - IE: Consistency error affecting adjustable parameters");
            break;
        case 8562:
            /* A08562 - PROFINET: Syntax error in configuration file */
            return QObject::tr("A08562 - PROFINET: Syntax error in configuration file");
            break;
        case 8563:
            /* A08563 - PROFINET: Consistency error affecting adjustable parameters */
            return QObject::tr("A08563 - PROFINET: Consistency error affecting adjustable parameters");
            break;
        case 8564:
            /* A08564 - PN/COMM BOARD: syntax error in the configuration file */
            return QObject::tr("A08564 - PN/COMM BOARD: syntax error in the configuration file");
            break;
        case 8565:
            /* A08565 - PNCOMM BOARD: Consistency error affecting adjustable parameters */
            return QObject::tr("A08565 - PNCOMM BOARD: Consistency error affecting adjustable parameters");
            break;
        case 8700:
            /* F08700 (A) - CAN: Communications error */
            return QObject::tr("F08700 (A) - CAN: Communications error");
            break;
        case 8701:
            /* F08701 - CAN: NMT state change */
            return QObject::tr("F08701 - CAN: NMT state change");
            break;
        case 8702:
            /* F08702 (A) - CAN: RPDO Timeout */
            return QObject::tr("F08702 (A) - CAN: RPDO Timeout");
            break;
        case 8703:
            /* F08703 (A) - CAN: Maximum number of drive objects exceeded */
            return QObject::tr("F08703 (A) - CAN: Maximum number of drive objects exceeded");
            break;
        case 8751:
            /* A08751 (N) - CAN: Telegram loss */
            return QObject::tr("A08751 (N) - CAN: Telegram loss");
            break;
        case 8752:
            /* A08752 - CAN: Error counter for error passive exceeded */
            return QObject::tr("A08752 - CAN: Error counter for error passive exceeded");
            break;
        case 8753:
            /* A08753 - CAN: Message buffer overflow */
            return QObject::tr("A08753 - CAN: Message buffer overflow");
            break;
        case 8754:
            /* A08754 - CAN: Incorrect communications mode */
            return QObject::tr("A08754 - CAN: Incorrect communications mode");
            break;
        case 8755:
            /* A08755 - CAN: Object cannot be mapped */
            return QObject::tr("A08755 - CAN: Object cannot be mapped");
            break;
        case 8756:
            /* A08756 - CAN: Number of mapped bytes exceeded */
            return QObject::tr("A08756 - CAN: Number of mapped bytes exceeded");
            break;
        case 8757:
            /* A08757 - CAN: Set COB-ID invalid */
            return QObject::tr("A08757 - CAN: Set COB-ID invalid");
            break;
        case 8758:
            /* A08758 - CAN: Maximum number of valid PDO exceeded */
            return QObject::tr("A08758 - CAN: Maximum number of valid PDO exceeded");
            break;
        case 8759:
            /* A08759 - CAN: PDO COB-ID already available */
            return QObject::tr("A08759 - CAN: PDO COB-ID already available");
            break;
        case 8760:
            /* A08760 - CAN: maximum size of the IF PZD exceeded */
            return QObject::tr("A08760 - CAN: maximum size of the IF PZD exceeded");
            break;
        case 8800:
            /* A08800 - PROFIenergy energy-saving mode active */
            return QObject::tr("A08800 - PROFIenergy energy-saving mode active");
            break;
        case 9000:
            /* A09000 - Web server user incorrectly configured */
            return QObject::tr("A09000 - Web server user incorrectly configured");
            break;
        case 13000:
            /* F13000 - License not adequate */
            return QObject::tr("F13000 - License not adequate");
            break;
        case 13001:
            /* A13001 - Error in license checksum */
            return QObject::tr("A13001 - Error in license checksum");
            break;
        case 13009:
            /* F13009 - Licensing Technology Extension not licensed */
            return QObject::tr("F13009 - Licensing Technology Extension not licensed");
            break;
        case 13010:
            /* F13010 - Licensing function module not licensed */
            return QObject::tr("F13010 - Licensing function module not licensed");
            break;
        case 13020:
            /* F13020 - Licensing not sufficient in the control */
            return QObject::tr("F13020 - Licensing not sufficient in the control");
            break;
        case 13021:
            /* A13021 - Licensing for output frequencies > 550 Hz missing */
            return QObject::tr("A13021 - Licensing for output frequencies > 550 Hz missing");
            break;
        case 13030:
            /* A13030 - Trial License activated */
            return QObject::tr("A13030 - Trial License activated");
            break;
        case 13031:
            /* A13031 - Trial License period expired */
            return QObject::tr("A13031 - Trial License period expired");
            break;
        case 13032:
            /* A13032 - Trial License last period activated */
            return QObject::tr("A13032 - Trial License last period activated");
            break;
        case 13033:
            /* A13033 - Trial License last period expired */
            return QObject::tr("A13033 - Trial License last period expired");
            break;
        case 13100:
            /* F13100 - Know-how protection: Copy protection error */
            return QObject::tr("F13100 - Know-how protection: Copy protection error");
            break;
        case 13101:
            /* F13101 - Know-how protection: Copy protection cannot be activated */
            return QObject::tr("F13101 - Know-how protection: Copy protection cannot be activated");
            break;
        case 13102:
            /* F13102 - Know-how protection: Consistency error of the protected data */
            return QObject::tr("F13102 - Know-how protection: Consistency error of the protected data");
            break;
        case 30001:
            /* F30001 - Power unit: Overcurrent */
            return QObject::tr("F30001 - Power unit: Overcurrent");
            break;
        case 30002:
            /* F30002 - Power unit: DC link voltage overvoltage */
            return QObject::tr("F30002 - Power unit: DC link voltage overvoltage");
            break;
        case 30003:
            /* F30003 - Power unit: DC link voltage undervoltage */
            return QObject::tr("F30003 - Power unit: DC link voltage undervoltage");
            break;
        case 30004:
            /* F30004 - Power unit: Overtemperature heat sink AC inverter */
            return QObject::tr("F30004 - Power unit: Overtemperature heat sink AC inverter");
            break;
        case 30005:
            /* F30005 - Power unit: Overload I2t */
            return QObject::tr("F30005 - Power unit: Overload I2t");
            break;
        case 30006:
            /* F30006 - Power unit: Thyristor Control Board */
            return QObject::tr("F30006 - Power unit: Thyristor Control Board");
            break;
        case 30008:
            /* F30008 - Power unit: Sign-of-life error cyclic data */
            return QObject::tr("F30008 - Power unit: Sign-of-life error cyclic data");
            break;
        case 30010:
            /* A30010 (F) - Power unit: Sign-of-life error cyclic data */
            return QObject::tr("A30010 (F) - Power unit: Sign-of-life error cyclic data");
            break;
        case 30011:
            /* F30011 - Power unit: Line phase failure in main circuit */
            return QObject::tr("F30011 - Power unit: Line phase failure in main circuit");
            break;
        case 30012:
            /* F30012 - Power unit: Temperature sensor wire breakage */
            return QObject::tr("F30012 - Power unit: Temperature sensor wire breakage");
            break;
        case 30013:
            /* F30013 - Power unit: Temperature sensor short circuit */
            return QObject::tr("F30013 - Power unit: Temperature sensor short circuit");
            break;
        case 30015:
            /* F30015 (N, A) - Power unit: Phase failure motor cable */
            return QObject::tr("F30015 (N, A) - Power unit: Phase failure motor cable");
            break;
        case 30016:
            /* A30016 (N) - Power unit: Load supply switched off */
            return QObject::tr("A30016 (N) - Power unit: Load supply switched off");
            break;
        case 30017:
            /* F30017 - Power unit: 26.5 V supply voltage fault */
            return QObject::tr("F30017 - Power unit: 26.5 V supply voltage fault");
            break;
        case 30020:
            /* F30020 - Power unit: Configuration not supported */
            return QObject::tr("F30020 - Power unit: Configuration not supported");
            break;
        case 30021:
            /* F30021 - Power unit: Ground fault */
            return QObject::tr("F30021 - Power unit: Ground fault");
            break;
        case 30022:
            /* F30022 - Power unit: Monitoring U_ce */
            return QObject::tr("F30022 - Power unit: Monitoring U_ce");
            break;
        case 30024:
            /* F30024 - Power unit: Overtemperature thermal model */
            return QObject::tr("F30024 - Power unit: Overtemperature thermal model");
            break;
        case 30025:
            /* F30025 - Power unit: Chip overtemperature */
            return QObject::tr("F30025 - Power unit: Chip overtemperature");
            break;
        case 30027:
            /* F30027 - Power unit: Precharging DC link time monitoring */
            return QObject::tr("F30027 - Power unit: Precharging DC link time monitoring");
            break;
        case 30030:
            /* A30030 - Power unit: Internal overtemperature alarm */
            return QObject::tr("A30030 - Power unit: Internal overtemperature alarm");
            break;
        case 30031:
            /* A30031 - Power unit: Hardware current limiting in phase U */
            return QObject::tr("A30031 - Power unit: Hardware current limiting in phase U");
            break;
        case 30032:
            /* A30032 - Power unit: Hardware current limiting in phase V */
            return QObject::tr("A30032 - Power unit: Hardware current limiting in phase V");
            break;
        case 30033:
            /* A30033 - Power unit: Hardware current limiting in phase W */
            return QObject::tr("A30033 - Power unit: Hardware current limiting in phase W");
            break;
        case 30034:
            /* A30034 - Power unit: Internal overtemperature */
            return QObject::tr("A30034 - Power unit: Internal overtemperature");
            break;
        case 30035:
            /* F30035 - Power unit: Air intake overtemperature */
            return QObject::tr("F30035 - Power unit: Air intake overtemperature");
            break;
        case 30036:
            /* F30036 - Power unit: Internal overtemperature */
            return QObject::tr("F30036 - Power unit: Internal overtemperature");
            break;
        case 30037:
            /* F30037 - Power unit: Rectifier overtemperature */
            return QObject::tr("F30037 - Power unit: Rectifier overtemperature");
            break;
        case 30038:
            /* A30038 - Power unit: Capacitor fan monitoring */
            return QObject::tr("A30038 - Power unit: Capacitor fan monitoring");
            break;
        case 30039:
            /* F30039 - Power unit: Failure capacitor fan */
            return QObject::tr("F30039 - Power unit: Failure capacitor fan");
            break;
        case 30040:
            /* F30040 - Power unit: Undervolt 24 V */
            return QObject::tr("F30040 - Power unit: Undervolt 24 V");
            break;
        case 30041:
            /* A30041 (F) - Power unit: Undervoltage 24 V alarm */
            return QObject::tr("A30041 (F) - Power unit: Undervoltage 24 V alarm");
            break;
        case 30042:
            /* A30042 - Power unit: Fan has reached the maximum operating hours */
            return QObject::tr("A30042 - Power unit: Fan has reached the maximum operating hours");
            break;
        case 30043:
            /* F30043 - Power unit: Overvolt 24/48 V */
            return QObject::tr("F30043 - Power unit: Overvolt 24/48 V");
            break;
        case 30044:
            /* A30044 (F) - Power unit: Overvoltage 24 V alarm */
            return QObject::tr("A30044 (F) - Power unit: Overvoltage 24 V alarm");
            break;
        case 30045:
            /* F30045 - Power unit: Supply undervoltage */
            return QObject::tr("F30045 - Power unit: Supply undervoltage");
            break;
        case 30046:
            /* A30046 (F) - Power unit: Undervoltage alarm */
            return QObject::tr("A30046 (F) - Power unit: Undervoltage alarm");
            break;
        case 30047:
            /* F30047 - Cooling unit: Cooling medium flow rate too low */
            return QObject::tr("F30047 - Cooling unit: Cooling medium flow rate too low");
            break;
        case 30048:
            /* A30048 - Power unit: fan defective */
            return QObject::tr("A30048 - Power unit: fan defective");
            break;
        case 30049:
            /* A30049 - Power unit: Internal fan faulty */
            return QObject::tr("A30049 - Power unit: Internal fan faulty");
            break;
        case 30050:
            /* F30050 - Power unit: 24 V supply overvoltage */
            return QObject::tr("F30050 - Power unit: 24 V supply overvoltage");
            break;
        case 30051:
            /* F30051 - Power unit: Motor holding brake short circuit detected */
            return QObject::tr("F30051 - Power unit: Motor holding brake short circuit detected");
            break;
        case 30052:
            /* F30052 - EEPROM data error */
            return QObject::tr("F30052 - EEPROM data error");
            break;
        case 30053:
            /* F30053 - FPGA data faulty */
            return QObject::tr("F30053 - FPGA data faulty");
            break;
        case 30054:
            /* A30054 (F, N) - Power unit: Undervoltage when opening the brake */
            return QObject::tr("A30054 (F, N) - Power unit: Undervoltage when opening the brake");
            break;
        case 30055:
            /* F30055 - Power unit: Braking chopper overcurrent */
            return QObject::tr("F30055 - Power unit: Braking chopper overcurrent");
            break;
        case 30057:
            /* A30057 - Power unit: Line asymmetry */
            return QObject::tr("A30057 - Power unit: Line asymmetry");
            break;
        case 30058:
            /* F30058 (N, A) - Power unit: heat sink fan defective */
            return QObject::tr("F30058 (N, A) - Power unit: heat sink fan defective");
            break;
        case 30059:
            /* F30059 - Power unit: Internal fan faulty */
            return QObject::tr("F30059 - Power unit: Internal fan faulty");
            break;
        case 30060:
            /* F30060 (A) - Precharge contactor state monitoring */
            return QObject::tr("F30060 (A) - Precharge contactor state monitoring");
            break;
        case 30061:
            /* F30061 (A) - Bridging contactor monitoring */
            return QObject::tr("F30061 (A) - Bridging contactor monitoring");
            break;
        case 30065:
            /* A30065 (F, N) - Voltage measured values not plausible */
            return QObject::tr("A30065 (F, N) - Voltage measured values not plausible");
            break;
        case 30066:
            /* A30066 (N) - Power unit: overtemperature, alarm capacitor air discharge */
            return QObject::tr("A30066 (N) - Power unit: overtemperature, alarm capacitor air discharge");
            break;
        case 30067:
            /* F30067 - Power unit: overtemperature, fault capacitor air discharge */
            return QObject::tr("F30067 - Power unit: overtemperature, fault capacitor air discharge");
            break;
        case 30068:
            /* F30068 - Power unit: undertemperature inverter heat sink */
            return QObject::tr("F30068 - Power unit: undertemperature inverter heat sink");
            break;
        case 30070:
            /* F30070 - Cycle requested by the power unit module not supported */
            return QObject::tr("F30070 - Cycle requested by the power unit module not supported");
            break;
        case 30071:
            /* F30071 - No new actual values received from the power unit */
            return QObject::tr("F30071 - No new actual values received from the power unit");
            break;
        case 30072:
            /* F30072 - Setpoints can no longer be transferred to the power unit */
            return QObject::tr("F30072 - Setpoints can no longer be transferred to the power unit");
            break;
        case 30073:
            /* A30073 (N) - Actual value/setpoint preprocessing no longer synchronous */
            return QObject::tr("A30073 (N) - Actual value/setpoint preprocessing no longer synchronous");
            break;
        case 30074:
            /* F30074 (A) - Communication error between the Control Unit and Power Module */
            return QObject::tr("F30074 (A) - Communication error between the Control Unit and Power Module");
            break;
        case 30075:
            /* F30075 - Configuration of the power unit unsuccessful */
            return QObject::tr("F30075 - Configuration of the power unit unsuccessful");
            break;
        case 30076:
            /* A30076 (N) - Power unit: thermal overload internal braking resistor alarm */
            return QObject::tr("A30076 (N) - Power unit: thermal overload internal braking resistor alarm");
            break;
        case 30077:
            /* A30077 (N) - Power unit: thermal overload internal braking resistor */
            return QObject::tr("A30077 (N) - Power unit: thermal overload internal braking resistor");
            break;
        case 30078:
            /* F30078 - Power unit: defective fan or line reactor has overheated */
            return QObject::tr("F30078 - Power unit: defective fan or line reactor has overheated");
            break;
        case 30080:
            /* F30080 - Power unit: Current increasing too quickly */
            return QObject::tr("F30080 - Power unit: Current increasing too quickly");
            break;
        case 30081:
            /* F30081 - Power unit: Switching operations too frequent */
            return QObject::tr("F30081 - Power unit: Switching operations too frequent");
            break;
        case 30082:
            /* A30082 - Power unit: cooling medium flow rate too low alarm threshold */
            return QObject::tr("A30082 - Power unit: cooling medium flow rate too low alarm threshold");
            break;
        case 30083:
            /* F30083 - Power unit: cooling medium flow rate too low fault threshold */
            return QObject::tr("F30083 - Power unit: cooling medium flow rate too low fault threshold");
            break;
        case 30086:
            /* A30086 - Power unit: coolant temperature has exceeded the alarm threshold */
            return QObject::tr("A30086 - Power unit: coolant temperature has exceeded the alarm threshold");
            break;
        case 30087:
            /* F30087 - Power unit: coolant temperature has exceeded the fault threshold */
            return QObject::tr("F30087 - Power unit: coolant temperature has exceeded the fault threshold");
            break;
        case 30105:
            /* F30105 - PU: Actual value sensing fault */
            return QObject::tr("F30105 - PU: Actual value sensing fault");
            break;
        case 30314:
            /* F30314 - Power unit: 24 V power supply overloaded by PM */
            return QObject::tr("F30314 - Power unit: 24 V power supply overloaded by PM");
            break;
        case 30315:
            /* A30315 (F) - Power unit: 24 V power supply overloaded by PM */
            return QObject::tr("A30315 (F) - Power unit: 24 V power supply overloaded by PM");
            break;
        case 30502:
            /* A30502 - Power unit: DC link overvoltage */
            return QObject::tr("A30502 - Power unit: DC link overvoltage");
            break;
        case 30600:
            /* F30600 - SI P2: STOP A initiated */
            return QObject::tr("F30600 - SI P2: STOP A initiated");
            break;
        case 30611:
            /* F30611 (A) - SI P2: Defect in a monitoring channel */
            return QObject::tr("F30611 (A) - SI P2: Defect in a monitoring channel");
            break;
        case 30620:
            /* N30620 (F, A) - SI P2: Safe Torque Off active */
            return QObject::tr("N30620 (F, A) - SI P2: Safe Torque Off active");
            break;
        case 30621:
            /* N30621 (F, A) - SI P2: Safe Stop 1 active */
            return QObject::tr("N30621 (F, A) - SI P2: Safe Stop 1 active");
            break;
        case 30625:
            /* F30625 - SI P2: Sign-of-life error in safety data */
            return QObject::tr("F30625 - SI P2: Sign-of-life error in safety data");
            break;
        case 30630:
            /* F30630 - SI P2: Brake control error */
            return QObject::tr("F30630 - SI P2: Brake control error");
            break;
        case 30631:
            /* F30631 - Brake control: External release active */
            return QObject::tr("F30631 - Brake control: External release active");
            break;
        case 30632:
            /* F30632 - SI P2: shutoff valve control/feedback signal error */
            return QObject::tr("F30632 - SI P2: shutoff valve control/feedback signal error");
            break;
        case 30640:
            /* A30640 (F) - SI P2: Fault in the switch-off signal path of the second channel */
            return QObject::tr("A30640 (F) - SI P2: Fault in the switch-off signal path of the second channel");
            break;
        case 30649:
            /* F30649 - SI P2: Internal software error */
            return QObject::tr("F30649 - SI P2: Internal software error");
            break;
        case 30650:
            /* F30650 - SI P2: Acceptance test required */
            return QObject::tr("F30650 - SI P2: Acceptance test required");
            break;
        case 30651:
            /* F30651 - SI P2: Synchronization with Control Unit unsuccessful */
            return QObject::tr("F30651 - SI P2: Synchronization with Control Unit unsuccessful");
            break;
        case 30652:
            /* F30652 - SI P2: Illegal monitoring clock cycle */
            return QObject::tr("F30652 - SI P2: Illegal monitoring clock cycle");
            break;
        case 30655:
            /* F30655 - SI P2: Align monitoring functions */
            return QObject::tr("F30655 - SI P2: Align monitoring functions");
            break;
        case 30656:
            /* F30656 - SI P2: Motor Module parameter error */
            return QObject::tr("F30656 - SI P2: Motor Module parameter error");
            break;
        case 30657:
            /* F30657 - SI P2: PROFIsafe telegram number invalid */
            return QObject::tr("F30657 - SI P2: PROFIsafe telegram number invalid");
            break;
        case 30659:
            /* F30659 - SI P2: Write request for parameter rejected */
            return QObject::tr("F30659 - SI P2: Write request for parameter rejected");
            break;
        case 30664:
            /* F30664 - Error while booting */
            return QObject::tr("F30664 - Error while booting");
            break;
        case 30665:
            /* F30665 - SI P2: System is defective */
            return QObject::tr("F30665 - SI P2: System is defective");
            break;
        case 30666:
            /* A30666 (F) - SI Motion P2: Steady-state (static) 1 signal at the F-DI for safe acknowledgment */
            return QObject::tr("A30666 (F) - SI Motion P2: Steady-state (static) 1 signal at the F-DI for safe acknowledgment");
            break;
        case 30672:
            /* F30672 - SI P2: Control Unit software incompatible */
            return QObject::tr("F30672 - SI P2: Control Unit software incompatible");
            break;
        case 30674:
            /* F30674 - SI Motion P2: Safety function not supported by PROFIsafe telegram */
            return QObject::tr("F30674 - SI Motion P2: Safety function not supported by PROFIsafe telegram");
            break;
        case 30680:
            /* F30680 - SI Motion P2: Checksum error safety monitoring functions */
            return QObject::tr("F30680 - SI Motion P2: Checksum error safety monitoring functions");
            break;
        case 30681:
            /* F30681 - SI Motion P1: Incorrect parameter value */
            return QObject::tr("F30681 - SI Motion P1: Incorrect parameter value");
            break;
        case 30682:
            /* F30682 - SI Motion P2: Monitoring function not supported */
            return QObject::tr("F30682 - SI Motion P2: Monitoring function not supported");
            break;
        case 30683:
            /* F30683 - SI Motion P2: SOS/SLS enable missing */
            return QObject::tr("F30683 - SI Motion P2: SOS/SLS enable missing");
            break;
        case 30684:
            /* F30684 - SI Motion P2: Safely-Limited Position limit values interchanged */
            return QObject::tr("F30684 - SI Motion P2: Safely-Limited Position limit values interchanged");
            break;
        case 30685:
            /* F30685 - SI Motion P2: Safely-Limited Speed limit value too high */
            return QObject::tr("F30685 - SI Motion P2: Safely-Limited Speed limit value too high");
            break;
        case 30686:
            /* F30686 - SI Motion: Illegal parameterization cam position */
            return QObject::tr("F30686 - SI Motion: Illegal parameterization cam position");
            break;
        case 30688:
            /* F30688 - SI Motion P2: Actual value synchronization not permissible */
            return QObject::tr("F30688 - SI Motion P2: Actual value synchronization not permissible");
            break;
        case 30692:
            /* F30692 - SI Motion P2: Parameter value not permitted for encoderless */
            return QObject::tr("F30692 - SI Motion P2: Parameter value not permitted for encoderless");
            break;
        case 30693:
            /* A30693 (F) - SI P2: Safety parameter settings changed, warm restart/POWER ON required */
            return QObject::tr("A30693 (F) - SI P2: Safety parameter settings changed, warm restart/POWER ON required");
            break;
        case 30700:
            /* C30700 - SI Motion P2: STOP A initiated */
            return QObject::tr("C30700 - SI Motion P2: STOP A initiated");
            break;
        case 30701:
            /* C30701 - SI Motion P2: STOP B initiated */
            return QObject::tr("C30701 - SI Motion P2: STOP B initiated");
            break;
        case 30706:
            /* C30706 - SI Motion P2: SAM/SBR limit exceeded */
            return QObject::tr("C30706 - SI Motion P2: SAM/SBR limit exceeded");
            break;
        case 30707:
            /* C30707 - SI Motion P2: Tolerance for safe operating stop exceeded */
            return QObject::tr("C30707 - SI Motion P2: Tolerance for safe operating stop exceeded");
            break;
        case 30708:
            /* C30708 - SI Motion P2: STOP C initiated */
            return QObject::tr("C30708 - SI Motion P2: STOP C initiated");
            break;
        case 30709:
            /* C30709 - SI Motion P2: STOP D initiated */
            return QObject::tr("C30709 - SI Motion P2: STOP D initiated");
            break;
        case 30710:
            /* C30710 - SI Motion P2: STOP E initiated */
            return QObject::tr("C30710 - SI Motion P2: STOP E initiated");
            break;
        case 30711:
            /* C30711 - SI Motion P2: Defect in a monitoring channel */
            return QObject::tr("C30711 - SI Motion P2: Defect in a monitoring channel");
            break;
        case 30712:
            /* C30712 - SI Motion P2: Defect in F-IO processing */
            return QObject::tr("C30712 - SI Motion P2: Defect in F-IO processing");
            break;
        case 30714:
            /* C30714 - SI Motion P2: Safely-Limited Speed exceeded */
            return QObject::tr("C30714 - SI Motion P2: Safely-Limited Speed exceeded");
            break;
        case 30715:
            /* C30715 - SI Motion P2: Safely-Limited Position exceeded */
            return QObject::tr("C30715 - SI Motion P2: Safely-Limited Position exceeded");
            break;
        case 30716:
            /* C30716 - SI Motion P2: Tolerance for safe motion direction exceeded */
            return QObject::tr("C30716 - SI Motion P2: Tolerance for safe motion direction exceeded");
            break;
        case 30717:
            /* C30717 - SI Motion P2: SLA limit exceeded */
            return QObject::tr("C30717 - SI Motion P2: SLA limit exceeded");
            break;
        case 30730:
            /* C30730 - SI Motion P2: Reference block for dynamic Safely-Limited Speed invalid */
            return QObject::tr("C30730 - SI Motion P2: Reference block for dynamic Safely-Limited Speed invalid");
            break;
        case 30770:
            /* C30770 - SI Motion P2: Discrepancy error of the failsafe inputs/outputs */
            return QObject::tr("C30770 - SI Motion P2: Discrepancy error of the failsafe inputs/outputs");
            break;
        case 30772:
            /* A30772 - SI Motion P2: Test stop for failsafe digital outputs running */
            return QObject::tr("A30772 - SI Motion P2: Test stop for failsafe digital outputs running");
            break;
        case 30773:
            /* F30773 - SI Motion P2: Test stop failsafe digital output error */
            return QObject::tr("F30773 - SI Motion P2: Test stop failsafe digital output error");
            break;
        case 30788:
            /* A30788 - Automatic test stop: wait for STO deselection via SMM */
            return QObject::tr("A30788 - Automatic test stop: wait for STO deselection via SMM");
            break;
        case 30797:
            /* C30797 - SI Motion P2: Axis not safely referenced */
            return QObject::tr("C30797 - SI Motion P2: Axis not safely referenced");
            break;
        case 30798:
            /* C30798 - SI Motion P2: Test stop for motion monitoring functions running */
            return QObject::tr("C30798 - SI Motion P2: Test stop for motion monitoring functions running");
            break;
        case 30799:
            /* C30799 - SI Motion P2: Acceptance test mode active */
            return QObject::tr("C30799 - SI Motion P2: Acceptance test mode active");
            break;
        case 30800:
            /* N30800 (F) - Power unit: Group signal */
            return QObject::tr("N30800 (F) - Power unit: Group signal");
            break;
        case 30801:
            /* F30801 - Power unit DRIVE-CLiQ: Sign-of-life missing */
            return QObject::tr("F30801 - Power unit DRIVE-CLiQ: Sign-of-life missing");
            break;
        case 30802:
            /* F30802 - Power unit: Time slice overflow */
            return QObject::tr("F30802 - Power unit: Time slice overflow");
            break;
        case 30804:
            /* F30804 (N, A) - Power unit: CRC */
            return QObject::tr("F30804 (N, A) - Power unit: CRC");
            break;
        case 30805:
            /* F30805 - Power unit: EEPROM checksum error */
            return QObject::tr("F30805 - Power unit: EEPROM checksum error");
            break;
        case 30809:
            /* F30809 - Power unit: Switching information not valid */
            return QObject::tr("F30809 - Power unit: Switching information not valid");
            break;
        case 30810:
            /* A30810 (F) - Power unit: Watchdog timer */
            return QObject::tr("A30810 (F) - Power unit: Watchdog timer");
            break;
        case 30820:
            /* F30820 - Power unit DRIVE-CLiQ: Telegram error */
            return QObject::tr("F30820 - Power unit DRIVE-CLiQ: Telegram error");
            break;
        case 30835:
            /* F30835 - Power unit DRIVE-CLiQ: Cyclic data transfer error */
            return QObject::tr("F30835 - Power unit DRIVE-CLiQ: Cyclic data transfer error");
            break;
        case 30836:
            /* F30836 - Power unit DRIVE-CLiQ: Send error for DRIVE-CLiQ data */
            return QObject::tr("F30836 - Power unit DRIVE-CLiQ: Send error for DRIVE-CLiQ data");
            break;
        case 30837:
            /* F30837 - Power unit DRIVE-CLiQ: Component fault */
            return QObject::tr("F30837 - Power unit DRIVE-CLiQ: Component fault");
            break;
        case 30845:
            /* F30845 - Power unit DRIVE-CLiQ: Cyclic data transfer error */
            return QObject::tr("F30845 - Power unit DRIVE-CLiQ: Cyclic data transfer error");
            break;
        case 30850:
            /* F30850 - Power unit: Internal software error */
            return QObject::tr("F30850 - Power unit: Internal software error");
            break;
        case 30851:
            /* F30851 - Power unit DRIVE-CLiQ (CU): Sign-of-life missing */
            return QObject::tr("F30851 - Power unit DRIVE-CLiQ (CU): Sign-of-life missing");
            break;
        case 30853:
            /* A30853 - Power unit: Sign-of-life error cyclic data */
            return QObject::tr("A30853 - Power unit: Sign-of-life error cyclic data");
            break;
        case 30860:
            /* F30860 - Power unit DRIVE-CLiQ (CU): Telegram error */
            return QObject::tr("F30860 - Power unit DRIVE-CLiQ (CU): Telegram error");
            break;
        case 30875:
            /* F30875 - Power unit: power supply voltage failed */
            return QObject::tr("F30875 - Power unit: power supply voltage failed");
            break;
        case 30885:
            /* F30885 - CU DRIVE-CLiQ (CU): Cyclic data transfer error */
            return QObject::tr("F30885 - CU DRIVE-CLiQ (CU): Cyclic data transfer error");
            break;
        case 30886:
            /* F30886 - PU DRIVE-CLiQ (CU): Error when sending DRIVE-CLiQ data */
            return QObject::tr("F30886 - PU DRIVE-CLiQ (CU): Error when sending DRIVE-CLiQ data");
            break;
        case 30887:
            /* F30887 - Power unit DRIVE-CLiQ (CU): Component fault */
            return QObject::tr("F30887 - Power unit DRIVE-CLiQ (CU): Component fault");
            break;
        case 30895:
            /* F30895 - PU DRIVE-CLiQ (CU): Alternating cyclic data transfer error */
            return QObject::tr("F30895 - PU DRIVE-CLiQ (CU): Alternating cyclic data transfer error");
            break;
        case 30896:
            /* F30896 - Power unit DRIVE-CLiQ (CU): Inconsistent component properties */
            return QObject::tr("F30896 - Power unit DRIVE-CLiQ (CU): Inconsistent component properties");
            break;
        case 30899:
            /* F30899 (N, A) - Power unit: Unknown fault */
            return QObject::tr("F30899 (N, A) - Power unit: Unknown fault");
            break;
        case 30903:
            /* F30903 - Power unit: I2C bus error occurred */
            return QObject::tr("F30903 - Power unit: I2C bus error occurred");
            break;
        case 30907:
            /* F30907 - Power unit: FPGA configuration unsuccessful */
            return QObject::tr("F30907 - Power unit: FPGA configuration unsuccessful");
            break;
        case 30919:
            /* A30919 - Power unit: Temperature monitoring failed */
            return QObject::tr("A30919 - Power unit: Temperature monitoring failed");
            break;
        case 30920:
            /* A30920 (F) - Power unit: Temperature sensor fault */
            return QObject::tr("A30920 (F) - Power unit: Temperature sensor fault");
            break;
        case 30930:
            /* A30930 (N) - Power unit: Component trace has saved data */
            return QObject::tr("A30930 (N) - Power unit: Component trace has saved data");
            break;
        case 30950:
            /* F30950 - Power unit: Internal software error */
            return QObject::tr("F30950 - Power unit: Internal software error");
            break;
        case 30999:
            /* A30999 (F, N) - Power unit: Unknown alarm */
            return QObject::tr("A30999 (F, N) - Power unit: Unknown alarm");
            break;
        case 31100:
            /* F31100 (N, A) - Encoder 1: Zero mark distance error */
            return QObject::tr("F31100 (N, A) - Encoder 1: Zero mark distance error");
            break;
        case 31101:
            /* F31101 (N, A) - Encoder 1: Zero mark failed */
            return QObject::tr("F31101 (N, A) - Encoder 1: Zero mark failed");
            break;
        case 31103:
            /* F31103 (N, A) - Encoder 1: Signal level zero mark (track R) outside tolerance */
            return QObject::tr("F31103 (N, A) - Encoder 1: Signal level zero mark (track R) outside tolerance");
            break;
        case 31110:
            /* F31110 (N, A) - Encoder 1: Serial communications error */
            return QObject::tr("F31110 (N, A) - Encoder 1: Serial communications error");
            break;
        case 31111:
            /* F31111 (N, A) - Encoder 1: Encoder signals an internal error (detailed information) */
            return QObject::tr("F31111 (N, A) - Encoder 1: Encoder signals an internal error (detailed information)");
            break;
        case 31112:
            /* F31112 (N, A) - Encoder 1: Encoder signals an internal error */
            return QObject::tr("F31112 (N, A) - Encoder 1: Encoder signals an internal error");
            break;
        case 31115:
            /* F31115 (N, A) - Encoder 1: Signal level track A or B too low */
            return QObject::tr("F31115 (N, A) - Encoder 1: Signal level track A or B too low");
            break;
        case 31116:
            /* F31116 (N, A) - Encoder 1: Signal level track A or B too low */
            return QObject::tr("F31116 (N, A) - Encoder 1: Signal level track A or B too low");
            break;
        case 31117:
            /* F31117 (N, A) - Encoder 1: Inversion error signals A/B/R */
            return QObject::tr("F31117 (N, A) - Encoder 1: Inversion error signals A/B/R");
            break;
        case 31118:
            /* F31118 (N, A) - Encoder 1: Speed change not plausible */
            return QObject::tr("F31118 (N, A) - Encoder 1: Speed change not plausible");
            break;
        case 31120:
            /* F31120 (N, A) - Encoder 1: Encoder power supply fault */
            return QObject::tr("F31120 (N, A) - Encoder 1: Encoder power supply fault");
            break;
        case 31121:
            /* F31121 (N, A) - Encoder 1: Determined commutation position incorrect */
            return QObject::tr("F31121 (N, A) - Encoder 1: Determined commutation position incorrect");
            break;
        case 31122:
            /* F31122 - Encoder 1: Sensor Module hardware fault */
            return QObject::tr("F31122 - Encoder 1: Sensor Module hardware fault");
            break;
        case 31123:
            /* F31123 (N, A) - Encoder 1: Signal level A/B outside tolerance */
            return QObject::tr("F31123 (N, A) - Encoder 1: Signal level A/B outside tolerance");
            break;
        case 31125:
            /* F31125 (N, A) - Encoder 1: Signal level track A or B too high */
            return QObject::tr("F31125 (N, A) - Encoder 1: Signal level track A or B too high");
            break;
        case 31126:
            /* F31126 (N, A) - Encoder 1: Signal level track A or B too high */
            return QObject::tr("F31126 (N, A) - Encoder 1: Signal level track A or B too high");
            break;
        case 31129:
            /* F31129 (N, A) - Encoder 1: Position difference hall sensor/track C/D and A/B too large */
            return QObject::tr("F31129 (N, A) - Encoder 1: Position difference hall sensor/track C/D and A/B too large");
            break;
        case 31130:
            /* F31130 (N, A) - Encoder 1: Zero mark and position error from the coarse synchronization */
            return QObject::tr("F31130 (N, A) - Encoder 1: Zero mark and position error from the coarse synchronization");
            break;
        case 31131:
            /* F31131 (N, A) - Encoder 1: Position deviation incremental/absolute too high */
            return QObject::tr("F31131 (N, A) - Encoder 1: Position deviation incremental/absolute too high");
            break;
        case 31135:
            /* F31135 - Encoder 1: Fault when determining the position (single turn) */
            return QObject::tr("F31135 - Encoder 1: Fault when determining the position (single turn)");
            break;
        case 31136:
            /* F31136 - Encoder 1: Fault when determining the position (multiturn) */
            return QObject::tr("F31136 - Encoder 1: Fault when determining the position (multiturn)");
            break;
        case 31137:
            /* F31137 - Encoder 1: Fault when determining the position (single turn) */
            return QObject::tr("F31137 - Encoder 1: Fault when determining the position (single turn)");
            break;
        case 31138:
            /* F31138 - Encoder 1: Fault when determining the position (multiturn) */
            return QObject::tr("F31138 - Encoder 1: Fault when determining the position (multiturn)");
            break;
        case 31142:
            /* F31142 (N, A) - Encoder 1: Battery voltage fault */
            return QObject::tr("F31142 (N, A) - Encoder 1: Battery voltage fault");
            break;
        case 31150:
            /* F31150 (N, A) - Encoder 1: Initialization error */
            return QObject::tr("F31150 (N, A) - Encoder 1: Initialization error");
            break;
        case 31151:
            /* F31151 (N, A) - Encoder 1: Encoder speed for initialization AB too high */
            return QObject::tr("F31151 (N, A) - Encoder 1: Encoder speed for initialization AB too high");
            break;
        case 31152:
            /* F31152 (N, A) - Encoder 1: Max. signal frequency (track A/B) exceeded */
            return QObject::tr("F31152 (N, A) - Encoder 1: Max. signal frequency (track A/B) exceeded");
            break;
        case 31153:
            /* F31153 (N, A) - Encoder 1: Identification error */
            return QObject::tr("F31153 (N, A) - Encoder 1: Identification error");
            break;
        case 31160:
            /* F31160 (N, A) - Encoder 1: Analog sensor channel A failed */
            return QObject::tr("F31160 (N, A) - Encoder 1: Analog sensor channel A failed");
            break;
        case 31161:
            /* F31161 (N, A) - Encoder 1: Analog sensor channel B failed */
            return QObject::tr("F31161 (N, A) - Encoder 1: Analog sensor channel B failed");
            break;
        case 31163:
            /* F31163 (N, A) - Encoder 1: Analog sensor position value exceeds limit value */
            return QObject::tr("F31163 (N, A) - Encoder 1: Analog sensor position value exceeds limit value");
            break;
        case 31400:
            /* A31400 (F, N) - Encoder 1: Zero mark distance error (alarm threshold exceeded) */
            return QObject::tr("A31400 (F, N) - Encoder 1: Zero mark distance error (alarm threshold exceeded)");
            break;
        case 31401:
            /* A31401 (F, N) - Encoder 1: Zero mark failed (alarm threshold exceeded) */
            return QObject::tr("A31401 (F, N) - Encoder 1: Zero mark failed (alarm threshold exceeded)");
            break;
        case 31405:
            /* F31405 (N, A) - Encoder 1: Temperature in the encoder evaluation exceeded */
            return QObject::tr("F31405 (N, A) - Encoder 1: Temperature in the encoder evaluation exceeded");
            break;
        case 31407:
            /* A31407 (F, N) - Encoder 1: Function limit reached */
            return QObject::tr("A31407 (F, N) - Encoder 1: Function limit reached");
            break;
        case 31410:
            /* A31410 (F, N) - Encoder 1: Communication error (encoder and Sensor Module) */
            return QObject::tr("A31410 (F, N) - Encoder 1: Communication error (encoder and Sensor Module)");
            break;
        case 31411:
            /* A31411 (F, N) - Encoder 1: Encoder signals an internal alarm (detailed information) */
            return QObject::tr("A31411 (F, N) - Encoder 1: Encoder signals an internal alarm (detailed information)");
            break;
        case 31412:
            /* A31412 (F, N) - Encoder 1: Encoder signals an internal alarm */
            return QObject::tr("A31412 (F, N) - Encoder 1: Encoder signals an internal alarm");
            break;
        case 31414:
            /* A31414 (F, N) - Encoder 1: Signal level track C or D out of tolerance */
            return QObject::tr("A31414 (F, N) - Encoder 1: Signal level track C or D out of tolerance");
            break;
        case 31415:
            /* N31415 (F, A) - Encoder 1: Signal level track A or B outside tolerance (alarm) */
            return QObject::tr("N31415 (F, A) - Encoder 1: Signal level track A or B outside tolerance (alarm)");
            break;
        case 31418:
            /* A31418 (F, N) - Encoder 1: Speed change not plausible (alarm) */
            return QObject::tr("A31418 (F, N) - Encoder 1: Speed change not plausible (alarm)");
            break;
        case 31419:
            /* A31419 (F, N) - Encoder 1: Track A or B outside tolerance */
            return QObject::tr("A31419 (F, N) - Encoder 1: Track A or B outside tolerance");
            break;
        case 31421:
            /* A31421 (F, N) - Encoder 1: Determined commutation position incorrect (alarm) */
            return QObject::tr("A31421 (F, N) - Encoder 1: Determined commutation position incorrect (alarm)");
            break;
        case 31422:
            /* A31422 (F, N) - Encoder 1: Pulses per revolution square-wave encoder outside tolerance bandwidth */
            return QObject::tr("A31422 (F, N) - Encoder 1: Pulses per revolution square-wave encoder outside tolerance bandwidth");
            break;
        case 31429:
            /* A31429 (F, N) - Encoder 1: Position difference hall sensor/track C/D and A/B too large */
            return QObject::tr("A31429 (F, N) - Encoder 1: Position difference hall sensor/track C/D and A/B too large");
            break;
        case 31431:
            /* A31431 (F, N) - Encoder 1: Position deviation incremental/absolute too high (alarm) */
            return QObject::tr("A31431 (F, N) - Encoder 1: Position deviation incremental/absolute too high (alarm)");
            break;
        case 31432:
            /* A31432 (F, N) - Encoder 1: Rotor position adaptation corrects deviation */
            return QObject::tr("A31432 (F, N) - Encoder 1: Rotor position adaptation corrects deviation");
            break;
        case 31442:
            /* A31442 (F, N) - Encoder 1: Battery voltage alarm threshold reached */
            return QObject::tr("A31442 (F, N) - Encoder 1: Battery voltage alarm threshold reached");
            break;
        case 31443:
            /* A31443 (F, N) - Encoder 1: Signal level track C/D outside tolerance (alarm) */
            return QObject::tr("A31443 (F, N) - Encoder 1: Signal level track C/D outside tolerance (alarm)");
            break;
        case 31460:
            /* A31460 (N) - Encoder 1: Analog sensor channel A failed */
            return QObject::tr("A31460 (N) - Encoder 1: Analog sensor channel A failed");
            break;
        case 31461:
            /* A31461 (N) - Encoder 1: Analog sensor channel B failed */
            return QObject::tr("A31461 (N) - Encoder 1: Analog sensor channel B failed");
            break;
        case 31462:
            /* A31462 (N) - Encoder 1: Analog sensor no channel active */
            return QObject::tr("A31462 (N) - Encoder 1: Analog sensor no channel active");
            break;
        case 31463:
            /* A31463 (N) - Encoder 1: Analog sensor position value exceeds limit value */
            return QObject::tr("A31463 (N) - Encoder 1: Analog sensor position value exceeds limit value");
            break;
        case 31470:
            /* A31470 (F, N) - Encoder 1: Encoder signals an internal error (X521.7) */
            return QObject::tr("A31470 (F, N) - Encoder 1: Encoder signals an internal error (X521.7)");
            break;
        case 31500:
            /* F31500 (N, A) - Encoder 1: Position tracking traversing range exceeded */
            return QObject::tr("F31500 (N, A) - Encoder 1: Position tracking traversing range exceeded");
            break;
        case 31501:
            /* F31501 (N, A) - Encoder 1: Position tracking encoder position outside tolerance window */
            return QObject::tr("F31501 (N, A) - Encoder 1: Position tracking encoder position outside tolerance window");
            break;
        case 31502:
            /* F31502 (N, A) - Encoder 1: Encoder with measuring gear without valid signals */
            return QObject::tr("F31502 (N, A) - Encoder 1: Encoder with measuring gear without valid signals");
            break;
        case 31503:
            /* F31503 (N, A) - Encoder 1: Position tracking cannot be reset */
            return QObject::tr("F31503 (N, A) - Encoder 1: Position tracking cannot be reset");
            break;
        case 31700:
            /* A31700 - Encoder 1: Functional safety monitoring initiated */
            return QObject::tr("A31700 - Encoder 1: Functional safety monitoring initiated");
            break;
        case 31800:
            /* N31800 (F) - Encoder 1: Group signal */
            return QObject::tr("N31800 (F) - Encoder 1: Group signal");
            break;
        case 31801:
            /* F31801 (N, A) - Encoder 1 DRIVE-CLiQ: Sign-of-life missing */
            return QObject::tr("F31801 (N, A) - Encoder 1 DRIVE-CLiQ: Sign-of-life missing");
            break;
        case 31802:
            /* F31802 (N, A) - Encoder 1: Time slice overflow */
            return QObject::tr("F31802 (N, A) - Encoder 1: Time slice overflow");
            break;
        case 31804:
            /* F31804 (N, A) - Encoder 1: Sensor Module checksum error */
            return QObject::tr("F31804 (N, A) - Encoder 1: Sensor Module checksum error");
            break;
        case 31805:
            /* F31805 (N, A) - Encoder 1: EEPROM checksum error */
            return QObject::tr("F31805 (N, A) - Encoder 1: EEPROM checksum error");
            break;
        case 31806:
            /* F31806 (N, A) - Encoder 1: Initialization error */
            return QObject::tr("F31806 (N, A) - Encoder 1: Initialization error");
            break;
        case 31811:
            /* A31811 (F, N) - Encoder 1: Encoder serial number changed */
            return QObject::tr("A31811 (F, N) - Encoder 1: Encoder serial number changed");
            break;
        case 31812:
            /* F31812 (N, A) - Encoder 1: Requested cycle or RX-/TX timing not supported */
            return QObject::tr("F31812 (N, A) - Encoder 1: Requested cycle or RX-/TX timing not supported");
            break;
        case 31813:
            /* F31813 - Encoder 1: Hardware logic unit failed */
            return QObject::tr("F31813 - Encoder 1: Hardware logic unit failed");
            break;
        case 31820:
            /* F31820 (N, A) - Encoder 1 DRIVE-CLiQ: Telegram error */
            return QObject::tr("F31820 (N, A) - Encoder 1 DRIVE-CLiQ: Telegram error");
            break;
        case 31835:
            /* F31835 (N, A) - Encoder 1 DRIVE-CLiQ: Cyclic data transfer error */
            return QObject::tr("F31835 (N, A) - Encoder 1 DRIVE-CLiQ: Cyclic data transfer error");
            break;
        case 31836:
            /* F31836 (N, A) - Encoder 1 DRIVE-CLiQ: Send error for DRIVE-CLiQ data */
            return QObject::tr("F31836 (N, A) - Encoder 1 DRIVE-CLiQ: Send error for DRIVE-CLiQ data");
            break;
        case 31837:
            /* F31837 (N, A) - Encoder 1 DRIVE-CLiQ: Component fault */
            return QObject::tr("F31837 (N, A) - Encoder 1 DRIVE-CLiQ: Component fault");
            break;
        case 31845:
            /* F31845 (N, A) - Encoder 1 DRIVE-CLiQ: Cyclic data transfer error */
            return QObject::tr("F31845 (N, A) - Encoder 1 DRIVE-CLiQ: Cyclic data transfer error");
            break;
        case 31850:
            /* F31850 (N, A) - Encoder 1: Encoder evaluation internal software error */
            return QObject::tr("F31850 (N, A) - Encoder 1: Encoder evaluation internal software error");
            break;
        case 31851:
            /* F31851 (N, A) - Encoder 1 DRIVE-CLiQ (CU): Sign-of-life missing */
            return QObject::tr("F31851 (N, A) - Encoder 1 DRIVE-CLiQ (CU): Sign-of-life missing");
            break;
        case 31860:
            /* F31860 (N, A) - Encoder 1 DRIVE-CLiQ (CU): Telegram error */
            return QObject::tr("F31860 (N, A) - Encoder 1 DRIVE-CLiQ (CU): Telegram error");
            break;
        case 31875:
            /* F31875 (N, A) - Encoder 1: power supply voltage failed */
            return QObject::tr("F31875 (N, A) - Encoder 1: power supply voltage failed");
            break;
        case 31885:
            /* F31885 (N, A) - Encoder 1 DRIVE-CLiQ (CU): Cyclic data transfer error */
            return QObject::tr("F31885 (N, A) - Encoder 1 DRIVE-CLiQ (CU): Cyclic data transfer error");
            break;
        case 31886:
            /* F31886 (N, A) - Encoder 1 DRIVE-CLiQ (CU): Error when sending DRIVE-CLiQ data */
            return QObject::tr("F31886 (N, A) - Encoder 1 DRIVE-CLiQ (CU): Error when sending DRIVE-CLiQ data");
            break;
        case 31887:
            /* F31887 (N, A) - Encoder 1 DRIVE-CLiQ (CU): Component fault */
            return QObject::tr("F31887 (N, A) - Encoder 1 DRIVE-CLiQ (CU): Component fault");
            break;
        case 31895:
            /* F31895 (N, A) - Encoder 1 DRIVE-CLiQ (CU): Alternating cyclic data transfer error */
            return QObject::tr("F31895 (N, A) - Encoder 1 DRIVE-CLiQ (CU): Alternating cyclic data transfer error");
            break;
        case 31896:
            /* F31896 (N, A) - Encoder 1 DRIVE-CLiQ (CU): Inconsistent component properties */
            return QObject::tr("F31896 (N, A) - Encoder 1 DRIVE-CLiQ (CU): Inconsistent component properties");
            break;
        case 31899:
            /* F31899 (N, A) - Encoder 1: Unknown fault */
            return QObject::tr("F31899 (N, A) - Encoder 1: Unknown fault");
            break;
        case 31902:
            /* A31902 (F, N) - Encoder 1: SPI-BUS error occurred */
            return QObject::tr("A31902 (F, N) - Encoder 1: SPI-BUS error occurred");
            break;
        case 31903:
            /* A31903 (F, N) - Encoder 1: I2C-BUS error occurred */
            return QObject::tr("A31903 (F, N) - Encoder 1: I2C-BUS error occurred");
            break;
        case 31905:
            /* F31905 (N, A) - Encoder 1: Encoder parameterization error */
            return QObject::tr("F31905 (N, A) - Encoder 1: Encoder parameterization error");
            break;
        case 31912:
            /* F31912 - Encoder 1: Device combination is not permissible */
            return QObject::tr("F31912 - Encoder 1: Device combination is not permissible");
            break;
        case 31915:
            /* A31915 (F, N) - Encoder 1: Encoder configuration error */
            return QObject::tr("A31915 (F, N) - Encoder 1: Encoder configuration error");
            break;
        case 31916:
            /* F31916 (N, A) - Encoder 1: Encoder parameterization error */
            return QObject::tr("F31916 (N, A) - Encoder 1: Encoder parameterization error");
            break;
        case 31920:
            /* A31920 (F, N) - Encoder 1: Temperature sensor fault (motor) */
            return QObject::tr("A31920 (F, N) - Encoder 1: Temperature sensor fault (motor)");
            break;
        case 31930:
            /* A31930 (N) - Encoder 1: Data logger has saved data */
            return QObject::tr("A31930 (N) - Encoder 1: Data logger has saved data");
            break;
        case 31940:
            /* A31940 (F, N) - Encoder 1: Spindle sensor S1 voltage incorrect */
            return QObject::tr("A31940 (F, N) - Encoder 1: Spindle sensor S1 voltage incorrect");
            break;
        case 31950:
            /* F31950 - Encoder 1: Internal software error */
            return QObject::tr("F31950 - Encoder 1: Internal software error");
            break;
        case 31999:
            /* A31999 (F, N) - Encoder 1: Unknown alarm */
            return QObject::tr("A31999 (F, N) - Encoder 1: Unknown alarm");
            break;
        case 32100:
            /* F32100 (N, A) - Encoder 2: Zero mark distance error */
            return QObject::tr("F32100 (N, A) - Encoder 2: Zero mark distance error");
            break;
        case 32101:
            /* F32101 (N, A) - Encoder 2: Zero mark failed */
            return QObject::tr("F32101 (N, A) - Encoder 2: Zero mark failed");
            break;
        case 32103:
            /* F32103 (N, A) - Encoder 2: Signal level zero track (track R) outside tolerance */
            return QObject::tr("F32103 (N, A) - Encoder 2: Signal level zero track (track R) outside tolerance");
            break;
        case 32110:
            /* F32110 (N, A) - Encoder 2: Serial communications error */
            return QObject::tr("F32110 (N, A) - Encoder 2: Serial communications error");
            break;
        case 32111:
            /* F32111 (N, A) - Encoder 2: Encoder signals an internal error (detailed information) */
            return QObject::tr("F32111 (N, A) - Encoder 2: Encoder signals an internal error (detailed information)");
            break;
        case 32112:
            /* F32112 (N, A) - Encoder 2: Encoder signals an internal error */
            return QObject::tr("F32112 (N, A) - Encoder 2: Encoder signals an internal error");
            break;
        case 32115:
            /* F32115 (N, A) - Encoder 2: Signal level track A or B too low */
            return QObject::tr("F32115 (N, A) - Encoder 2: Signal level track A or B too low");
            break;
        case 32116:
            /* F32116 (N, A) - Encoder 2: Signal level track A or B too low */
            return QObject::tr("F32116 (N, A) - Encoder 2: Signal level track A or B too low");
            break;
        case 32117:
            /* F32117 (N, A) - Encoder 2: Inversion error signals A/B/R */
            return QObject::tr("F32117 (N, A) - Encoder 2: Inversion error signals A/B/R");
            break;
        case 32118:
            /* F32118 (N, A) - Encoder 2: Speed change not plausible */
            return QObject::tr("F32118 (N, A) - Encoder 2: Speed change not plausible");
            break;
        case 32120:
            /* F32120 (N, A) - Encoder 2: Encoder power supply fault */
            return QObject::tr("F32120 (N, A) - Encoder 2: Encoder power supply fault");
            break;
        case 32121:
            /* F32121 (N, A) - Encoder 2: Determined commutation position incorrect */
            return QObject::tr("F32121 (N, A) - Encoder 2: Determined commutation position incorrect");
            break;
        case 32122:
            /* F32122 - Encoder 2: Sensor Module hardware fault */
            return QObject::tr("F32122 - Encoder 2: Sensor Module hardware fault");
            break;
        case 32123:
            /* F32123 (N, A) - Encoder 2: Signal level A/B outside tolerance */
            return QObject::tr("F32123 (N, A) - Encoder 2: Signal level A/B outside tolerance");
            break;
        case 32125:
            /* F32125 (N, A) - Encoder 2: Signal level track A or B too high */
            return QObject::tr("F32125 (N, A) - Encoder 2: Signal level track A or B too high");
            break;
        case 32126:
            /* F32126 (N, A) - Encoder 2: Signal level track A or B too high */
            return QObject::tr("F32126 (N, A) - Encoder 2: Signal level track A or B too high");
            break;
        case 32129:
            /* F32129 (N, A) - Encoder 2: Position difference hall sensor/track C/D and A/B too large */
            return QObject::tr("F32129 (N, A) - Encoder 2: Position difference hall sensor/track C/D and A/B too large");
            break;
        case 32130:
            /* F32130 (N, A) - Encoder 2: Zero mark and position error from the coarse synchronization */
            return QObject::tr("F32130 (N, A) - Encoder 2: Zero mark and position error from the coarse synchronization");
            break;
        case 32131:
            /* F32131 (N, A) - Encoder 2: Position deviation incremental/absolute too high */
            return QObject::tr("F32131 (N, A) - Encoder 2: Position deviation incremental/absolute too high");
            break;
        case 32135:
            /* F32135 - Encoder 2: Fault when determining the position (single turn) */
            return QObject::tr("F32135 - Encoder 2: Fault when determining the position (single turn)");
            break;
        case 32136:
            /* F32136 - Encoder 2: Fault when determining the position (multiturn) */
            return QObject::tr("F32136 - Encoder 2: Fault when determining the position (multiturn)");
            break;
        case 32137:
            /* F32137 - Encoder 2: Fault when determining the position (single turn) */
            return QObject::tr("F32137 - Encoder 2: Fault when determining the position (single turn)");
            break;
        case 32138:
            /* F32138 - Encoder 2: Fault when determining the position (multiturn) */
            return QObject::tr("F32138 - Encoder 2: Fault when determining the position (multiturn)");
            break;
        case 32142:
            /* F32142 (N, A) - Encoder 2: Battery voltage fault */
            return QObject::tr("F32142 (N, A) - Encoder 2: Battery voltage fault");
            break;
        case 32150:
            /* F32150 (N, A) - Encoder 2: Initialization error */
            return QObject::tr("F32150 (N, A) - Encoder 2: Initialization error");
            break;
        case 32151:
            /* F32151 (N, A) - Encoder 2: Encoder speed for initialization AB too high */
            return QObject::tr("F32151 (N, A) - Encoder 2: Encoder speed for initialization AB too high");
            break;
        case 32152:
            /* F32152 (N, A) - Encoder 2: Max. signal frequency (track A/B) exceeded */
            return QObject::tr("F32152 (N, A) - Encoder 2: Max. signal frequency (track A/B) exceeded");
            break;
        case 32153:
            /* F32153 (N, A) - Encoder 2: Identification error */
            return QObject::tr("F32153 (N, A) - Encoder 2: Identification error");
            break;
        case 32160:
            /* F32160 (N, A) - Encoder 2: Analog sensor channel A failed */
            return QObject::tr("F32160 (N, A) - Encoder 2: Analog sensor channel A failed");
            break;
        case 32161:
            /* F32161 (N, A) - Encoder 2: Analog sensor channel B failed */
            return QObject::tr("F32161 (N, A) - Encoder 2: Analog sensor channel B failed");
            break;
        case 32163:
            /* F32163 (N, A) - Encoder 2: Analog sensor position value exceeds limit value */
            return QObject::tr("F32163 (N, A) - Encoder 2: Analog sensor position value exceeds limit value");
            break;
        case 32400:
            /* A32400 (F, N) - Encoder 2: Zero mark distance error (alarm threshold exceeded) */
            return QObject::tr("A32400 (F, N) - Encoder 2: Zero mark distance error (alarm threshold exceeded)");
            break;
        case 32401:
            /* A32401 (F, N) - Encoder 2: Zero mark failed (alarm threshold exceeded) */
            return QObject::tr("A32401 (F, N) - Encoder 2: Zero mark failed (alarm threshold exceeded)");
            break;
        case 32405:
            /* F32405 (N, A) - Encoder 2: Temperature in the encoder evaluation exceeded */
            return QObject::tr("F32405 (N, A) - Encoder 2: Temperature in the encoder evaluation exceeded");
            break;
        case 32407:
            /* A32407 (F, N) - Encoder 2: Function limit reached */
            return QObject::tr("A32407 (F, N) - Encoder 2: Function limit reached");
            break;
        case 32410:
            /* A32410 (F, N) - Encoder 2: Communication error (encoder and Sensor Module) */
            return QObject::tr("A32410 (F, N) - Encoder 2: Communication error (encoder and Sensor Module)");
            break;
        case 32411:
            /* A32411 (F, N) - Encoder 2: Encoder signals an internal alarm (detailed information) */
            return QObject::tr("A32411 (F, N) - Encoder 2: Encoder signals an internal alarm (detailed information)");
            break;
        case 32412:
            /* A32412 (F, N) - Encoder 2: Encoder signals an internal alarm */
            return QObject::tr("A32412 (F, N) - Encoder 2: Encoder signals an internal alarm");
            break;
        case 32414:
            /* A32414 (F, N) - Encoder 2: Signal level track C or D out of tolerance */
            return QObject::tr("A32414 (F, N) - Encoder 2: Signal level track C or D out of tolerance");
            break;
        case 32415:
            /* N32415 (F, A) - Encoder 2: Signal level track A or B outside tolerance (alarm) */
            return QObject::tr("N32415 (F, A) - Encoder 2: Signal level track A or B outside tolerance (alarm)");
            break;
        case 32418:
            /* A32418 (F, N) - Encoder 2: Speed change not plausible (alarm) */
            return QObject::tr("A32418 (F, N) - Encoder 2: Speed change not plausible (alarm)");
            break;
        case 32419:
            /* A32419 (F, N) - Encoder 2: Track A or B outside tolerance */
            return QObject::tr("A32419 (F, N) - Encoder 2: Track A or B outside tolerance");
            break;
        case 32421:
            /* A32421 (F, N) - Encoder 2: Determined commutation position incorrect (alarm) */
            return QObject::tr("A32421 (F, N) - Encoder 2: Determined commutation position incorrect (alarm)");
            break;
        case 32422:
            /* A32422 (F, N) - Encoder 2: Pulses per revolution square-wave encoder outside tolerance bandwidth */
            return QObject::tr("A32422 (F, N) - Encoder 2: Pulses per revolution square-wave encoder outside tolerance bandwidth");
            break;
        case 32429:
            /* A32429 (F, N) - Encoder 2: Position difference hall sensor/track C/D and A/B too large */
            return QObject::tr("A32429 (F, N) - Encoder 2: Position difference hall sensor/track C/D and A/B too large");
            break;
        case 32431:
            /* A32431 (F, N) - Encoder 2: Position deviation incremental/absolute too high (alarm) */
            return QObject::tr("A32431 (F, N) - Encoder 2: Position deviation incremental/absolute too high (alarm)");
            break;
        case 32432:
            /* A32432 (F, N) - Encoder 2: Rotor position adaptation corrects deviation */
            return QObject::tr("A32432 (F, N) - Encoder 2: Rotor position adaptation corrects deviation");
            break;
        case 32442:
            /* A32442 (F, N) - Encoder 2: Battery voltage alarm threshold reached */
            return QObject::tr("A32442 (F, N) - Encoder 2: Battery voltage alarm threshold reached");
            break;
        case 32443:
            /* A32443 (F, N) - Encoder 2: Signal level track C/D outside tolerance (alarm) */
            return QObject::tr("A32443 (F, N) - Encoder 2: Signal level track C/D outside tolerance (alarm)");
            break;
        case 32460:
            /* A32460 (N) - Encoder 2: Analog sensor channel A failed */
            return QObject::tr("A32460 (N) - Encoder 2: Analog sensor channel A failed");
            break;
        case 32461:
            /* A32461 (N) - Encoder 2: Analog sensor channel B failed */
            return QObject::tr("A32461 (N) - Encoder 2: Analog sensor channel B failed");
            break;
        case 32462:
            /* A32462 (N) - Encoder 2: Analog sensor no channel active */
            return QObject::tr("A32462 (N) - Encoder 2: Analog sensor no channel active");
            break;
        case 32463:
            /* A32463 (N) - Encoder 2: Analog sensor position value exceeds limit value */
            return QObject::tr("A32463 (N) - Encoder 2: Analog sensor position value exceeds limit value");
            break;
        case 32470:
            /* A32470 (F, N) - Encoder 2: Encoder signals an internal error (X521.7) */
            return QObject::tr("A32470 (F, N) - Encoder 2: Encoder signals an internal error (X521.7)");
            break;
        case 32500:
            /* F32500 (N, A) - Encoder 2: Position tracking traversing range exceeded */
            return QObject::tr("F32500 (N, A) - Encoder 2: Position tracking traversing range exceeded");
            break;
        case 32501:
            /* F32501 (N, A) - Encoder 2: Position tracking encoder position outside tolerance window */
            return QObject::tr("F32501 (N, A) - Encoder 2: Position tracking encoder position outside tolerance window");
            break;
        case 32502:
            /* F32502 (N, A) - Encoder 2: Encoder with measuring gear without valid signals */
            return QObject::tr("F32502 (N, A) - Encoder 2: Encoder with measuring gear without valid signals");
            break;
        case 32503:
            /* F32503 (N, A) - Encoder 2: Position tracking cannot be reset */
            return QObject::tr("F32503 (N, A) - Encoder 2: Position tracking cannot be reset");
            break;
        case 32700:
            /* A32700 - Encoder 2: Effectivity test does not supply the expected value */
            return QObject::tr("A32700 - Encoder 2: Effectivity test does not supply the expected value");
            break;
        case 32800:
            /* N32800 (F) - Encoder 2: Group signal */
            return QObject::tr("N32800 (F) - Encoder 2: Group signal");
            break;
        case 32801:
            /* F32801 (N, A) - Encoder 2 DRIVE-CLiQ: Sign-of-life missing */
            return QObject::tr("F32801 (N, A) - Encoder 2 DRIVE-CLiQ: Sign-of-life missing");
            break;
        case 32802:
            /* F32802 (N, A) - Encoder 2: Time slice overflow */
            return QObject::tr("F32802 (N, A) - Encoder 2: Time slice overflow");
            break;
        case 32804:
            /* F32804 (N, A) - Encoder 2: Sensor Module checksum error */
            return QObject::tr("F32804 (N, A) - Encoder 2: Sensor Module checksum error");
            break;
        case 32805:
            /* F32805 (N, A) - Encoder 2: EEPROM checksum error */
            return QObject::tr("F32805 (N, A) - Encoder 2: EEPROM checksum error");
            break;
        case 32806:
            /* F32806 (N, A) - Encoder 2: Initialization error */
            return QObject::tr("F32806 (N, A) - Encoder 2: Initialization error");
            break;
        case 32811:
            /* A32811 (F, N) - Encoder 2: Encoder serial number changed */
            return QObject::tr("A32811 (F, N) - Encoder 2: Encoder serial number changed");
            break;
        case 32812:
            /* F32812 (N, A) - Encoder 2: Requested cycle or RX-/TX timing not supported */
            return QObject::tr("F32812 (N, A) - Encoder 2: Requested cycle or RX-/TX timing not supported");
            break;
        case 32813:
            /* F32813 - Encoder 2: Hardware logic unit failed */
            return QObject::tr("F32813 - Encoder 2: Hardware logic unit failed");
            break;
        case 32820:
            /* F32820 (N, A) - Encoder 2 DRIVE-CLiQ: Telegram error */
            return QObject::tr("F32820 (N, A) - Encoder 2 DRIVE-CLiQ: Telegram error");
            break;
        case 32835:
            /* F32835 (N, A) - Encoder 2 DRIVE-CLiQ: Cyclic data transfer error */
            return QObject::tr("F32835 (N, A) - Encoder 2 DRIVE-CLiQ: Cyclic data transfer error");
            break;
        case 32836:
            /* F32836 (N, A) - Encoder 2 DRIVE-CLiQ: Send error for DRIVE-CLiQ data */
            return QObject::tr("F32836 (N, A) - Encoder 2 DRIVE-CLiQ: Send error for DRIVE-CLiQ data");
            break;
        case 32837:
            /* F32837 (N, A) - Encoder 2 DRIVE-CLiQ: Component fault */
            return QObject::tr("F32837 (N, A) - Encoder 2 DRIVE-CLiQ: Component fault");
            break;
        case 32845:
            /* F32845 (N, A) - Encoder 2 DRIVE-CLiQ: Cyclic data transfer error */
            return QObject::tr("F32845 (N, A) - Encoder 2 DRIVE-CLiQ: Cyclic data transfer error");
            break;
        case 32850:
            /* F32850 (N, A) - Encoder 2: Encoder evaluation internal software error */
            return QObject::tr("F32850 (N, A) - Encoder 2: Encoder evaluation internal software error");
            break;
        case 32851:
            /* F32851 (N, A) - Encoder 2 DRIVE-CLiQ (CU): Sign-of-life missing */
            return QObject::tr("F32851 (N, A) - Encoder 2 DRIVE-CLiQ (CU): Sign-of-life missing");
            break;
        case 32860:
            /* F32860 (N, A) - Encoder 2 DRIVE-CLiQ (CU): Telegram error */
            return QObject::tr("F32860 (N, A) - Encoder 2 DRIVE-CLiQ (CU): Telegram error");
            break;
        case 32875:
            /* F32875 (N, A) - Encoder 2: power supply voltage failed */
            return QObject::tr("F32875 (N, A) - Encoder 2: power supply voltage failed");
            break;
        case 32885:
            /* F32885 (N, A) - Encoder 2 DRIVE-CLiQ (CU): Cyclic data transfer error */
            return QObject::tr("F32885 (N, A) - Encoder 2 DRIVE-CLiQ (CU): Cyclic data transfer error");
            break;
        case 32886:
            /* F32886 (N, A) - Encoder 2 DRIVE-CLiQ (CU): Error when sending DRIVE-CLiQ data */
            return QObject::tr("F32886 (N, A) - Encoder 2 DRIVE-CLiQ (CU): Error when sending DRIVE-CLiQ data");
            break;
        case 32887:
            /* F32887 (N, A) - Encoder 2 DRIVE-CLiQ (CU): Component fault */
            return QObject::tr("F32887 (N, A) - Encoder 2 DRIVE-CLiQ (CU): Component fault");
            break;
        case 32895:
            /* F32895 (N, A) - Encoder 2 DRIVE-CLiQ (CU): Alternating cyclic data transfer error */
            return QObject::tr("F32895 (N, A) - Encoder 2 DRIVE-CLiQ (CU): Alternating cyclic data transfer error");
            break;
        case 32896:
            /* F32896 (N, A) - Encoder 2 DRIVE-CLiQ (CU): Inconsistent component properties */
            return QObject::tr("F32896 (N, A) - Encoder 2 DRIVE-CLiQ (CU): Inconsistent component properties");
            break;
        case 32899:
            /* F32899 (N, A) - Encoder 2: Unknown fault */
            return QObject::tr("F32899 (N, A) - Encoder 2: Unknown fault");
            break;
        case 32902:
            /* A32902 (F, N) - Encoder 2: SPI-BUS error occurred */
            return QObject::tr("A32902 (F, N) - Encoder 2: SPI-BUS error occurred");
            break;
        case 32903:
            /* A32903 (F, N) - Encoder 2: I2C-BUS error occurred */
            return QObject::tr("A32903 (F, N) - Encoder 2: I2C-BUS error occurred");
            break;
        case 32905:
            /* F32905 (N, A) - Encoder 2: Encoder parameterization error */
            return QObject::tr("F32905 (N, A) - Encoder 2: Encoder parameterization error");
            break;
        case 32912:
            /* F32912 - Encoder 2: Device combination is not permissible */
            return QObject::tr("F32912 - Encoder 2: Device combination is not permissible");
            break;
        case 32915:
            /* A32915 (F, N) - Encoder 2: Encoder configuration error */
            return QObject::tr("A32915 (F, N) - Encoder 2: Encoder configuration error");
            break;
        case 32916:
            /* F32916 (N, A) - Encoder 2: Encoder parameterization error */
            return QObject::tr("F32916 (N, A) - Encoder 2: Encoder parameterization error");
            break;
        case 32920:
            /* A32920 (F, N) - Encoder 2: Temperature sensor fault (motor) */
            return QObject::tr("A32920 (F, N) - Encoder 2: Temperature sensor fault (motor)");
            break;
        case 32930:
            /* A32930 (N) - Encoder 2: Data logger has saved data */
            return QObject::tr("A32930 (N) - Encoder 2: Data logger has saved data");
            break;
        case 32940:
            /* A32940 (F, N) - Encoder 2: Spindle sensor S1 voltage incorrect */
            return QObject::tr("A32940 (F, N) - Encoder 2: Spindle sensor S1 voltage incorrect");
            break;
        case 32950:
            /* F32950 - Encoder 2: Internal software error */
            return QObject::tr("F32950 - Encoder 2: Internal software error");
            break;
        case 32999:
            /* A32999 (F, N) - Encoder 2: Unknown alarm */
            return QObject::tr("A32999 (F, N) - Encoder 2: Unknown alarm");
            break;
        case 33100:
            /* F33100 (N, A) - Encoder 3: Zero mark distance error */
            return QObject::tr("F33100 (N, A) - Encoder 3: Zero mark distance error");
            break;
        case 33101:
            /* F33101 (N, A) - Encoder 3: Zero mark failed */
            return QObject::tr("F33101 (N, A) - Encoder 3: Zero mark failed");
            break;
        case 33103:
            /* F33103 (N, A) - Encoder 3: Signal level zero track (track R) outside tolerance */
            return QObject::tr("F33103 (N, A) - Encoder 3: Signal level zero track (track R) outside tolerance");
            break;
        case 33110:
            /* F33110 (N, A) - Encoder 3: Serial communications error */
            return QObject::tr("F33110 (N, A) - Encoder 3: Serial communications error");
            break;
        case 33111:
            /* F33111 (N, A) - Encoder 3: Encoder signals an internal error (detailed information) */
            return QObject::tr("F33111 (N, A) - Encoder 3: Encoder signals an internal error (detailed information)");
            break;
        case 33112:
            /* F33112 (N, A) - Encoder 3: Encoder signals an internal error */
            return QObject::tr("F33112 (N, A) - Encoder 3: Encoder signals an internal error");
            break;
        case 33115:
            /* F33115 (N, A) - Encoder 3: Signal level track A or B too low */
            return QObject::tr("F33115 (N, A) - Encoder 3: Signal level track A or B too low");
            break;
        case 33116:
            /* F33116 (N, A) - Encoder 3: Signal level track A or B too low */
            return QObject::tr("F33116 (N, A) - Encoder 3: Signal level track A or B too low");
            break;
        case 33117:
            /* F33117 (N, A) - Encoder 3: Inversion error signals A/B/R */
            return QObject::tr("F33117 (N, A) - Encoder 3: Inversion error signals A/B/R");
            break;
        case 33118:
            /* F33118 (N, A) - Encoder 3: Speed change not plausible */
            return QObject::tr("F33118 (N, A) - Encoder 3: Speed change not plausible");
            break;
        case 33120:
            /* F33120 (N, A) - Encoder 3: Encoder power supply fault */
            return QObject::tr("F33120 (N, A) - Encoder 3: Encoder power supply fault");
            break;
        case 33121:
            /* F33121 (N, A) - Encoder 3: Determined commutation position incorrect */
            return QObject::tr("F33121 (N, A) - Encoder 3: Determined commutation position incorrect");
            break;
        case 33122:
            /* F33122 - Encoder 3: Sensor Module hardware fault */
            return QObject::tr("F33122 - Encoder 3: Sensor Module hardware fault");
            break;
        case 33123:
            /* F33123 (N, A) - Encoder 3: Signal level A/B outside tolerance */
            return QObject::tr("F33123 (N, A) - Encoder 3: Signal level A/B outside tolerance");
            break;
        case 33125:
            /* F33125 (N, A) - Encoder 3: Signal level track A or B too high */
            return QObject::tr("F33125 (N, A) - Encoder 3: Signal level track A or B too high");
            break;
        case 33126:
            /* F33126 (N, A) - Encoder 3: Signal level track A or B too high */
            return QObject::tr("F33126 (N, A) - Encoder 3: Signal level track A or B too high");
            break;
        case 33129:
            /* F33129 (N, A) - Encoder 3: Position difference hall sensor/track C/D and A/B too large */
            return QObject::tr("F33129 (N, A) - Encoder 3: Position difference hall sensor/track C/D and A/B too large");
            break;
        case 33130:
            /* F33130 (N, A) - Encoder 3: Zero mark and position error from the coarse synchronization */
            return QObject::tr("F33130 (N, A) - Encoder 3: Zero mark and position error from the coarse synchronization");
            break;
        case 33131:
            /* F33131 (N, A) - Encoder 3: Position deviation incremental/absolute too high */
            return QObject::tr("F33131 (N, A) - Encoder 3: Position deviation incremental/absolute too high");
            break;
        case 33135:
            /* F33135 - Encoder 3: Fault when determining the position (singleturn) */
            return QObject::tr("F33135 - Encoder 3: Fault when determining the position (singleturn)");
            break;
        case 33136:
            /* F33136 - Encoder 3: Fault when determining the position (multiturn) */
            return QObject::tr("F33136 - Encoder 3: Fault when determining the position (multiturn)");
            break;
        case 33137:
            /* F33137 - Encoder 3: Fault when determining the position (singleturn) */
            return QObject::tr("F33137 - Encoder 3: Fault when determining the position (singleturn)");
            break;
        case 33138:
            /* F33138 - Encoder 3: Fault when determining the position (multiturn) */
            return QObject::tr("F33138 - Encoder 3: Fault when determining the position (multiturn)");
            break;
        case 33142:
            /* F33142 (N, A) - Encoder 3: Battery voltage fault */
            return QObject::tr("F33142 (N, A) - Encoder 3: Battery voltage fault");
            break;
        case 33150:
            /* F33150 (N, A) - Encoder 3: Initialization error */
            return QObject::tr("F33150 (N, A) - Encoder 3: Initialization error");
            break;
        case 33151:
            /* F33151 (N, A) - Encoder 3: Encoder speed for initialization AB too high */
            return QObject::tr("F33151 (N, A) - Encoder 3: Encoder speed for initialization AB too high");
            break;
        case 33152:
            /* F33152 (N, A) - Encoder 3: Max. signal frequency (track A/B) exceeded */
            return QObject::tr("F33152 (N, A) - Encoder 3: Max. signal frequency (track A/B) exceeded");
            break;
        case 33153:
            /* F33153 (N, A) - Encoder 3: Identification error */
            return QObject::tr("F33153 (N, A) - Encoder 3: Identification error");
            break;
        case 33160:
            /* F33160 (N, A) - Encoder 3: Analog sensor channel A failed */
            return QObject::tr("F33160 (N, A) - Encoder 3: Analog sensor channel A failed");
            break;
        case 33161:
            /* F33161 (N, A) - Encoder 3: Analog sensor channel B failed */
            return QObject::tr("F33161 (N, A) - Encoder 3: Analog sensor channel B failed");
            break;
        case 33163:
            /* F33163 (N, A) - Encoder 3: Analog sensor position value exceeds limit value */
            return QObject::tr("F33163 (N, A) - Encoder 3: Analog sensor position value exceeds limit value");
            break;
        case 33400:
            /* A33400 (F, N) - Encoder 3: Zero mark distance error (alarm threshold exceeded) */
            return QObject::tr("A33400 (F, N) - Encoder 3: Zero mark distance error (alarm threshold exceeded)");
            break;
        case 33401:
            /* A33401 (F, N) - Encoder 3: Zero mark failed (alarm threshold exceeded) */
            return QObject::tr("A33401 (F, N) - Encoder 3: Zero mark failed (alarm threshold exceeded)");
            break;
        case 33405:
            /* F33405 (N, A) - Encoder 3: Temperature in the encoder evaluation exceeded */
            return QObject::tr("F33405 (N, A) - Encoder 3: Temperature in the encoder evaluation exceeded");
            break;
        case 33407:
            /* A33407 (F, N) - Encoder 3: Function limit reached */
            return QObject::tr("A33407 (F, N) - Encoder 3: Function limit reached");
            break;
        case 33410:
            /* A33410 (F, N) - Encoder 3: Communication error (encoder and Sensor Module) */
            return QObject::tr("A33410 (F, N) - Encoder 3: Communication error (encoder and Sensor Module)");
            break;
        case 33411:
            /* A33411 (F, N) - Encoder 3: Encoder signals an internal alarm (detailed information) */
            return QObject::tr("A33411 (F, N) - Encoder 3: Encoder signals an internal alarm (detailed information)");
            break;
        case 33412:
            /* A33412 (F, N) - Encoder 3: Encoder signals an internal alarm */
            return QObject::tr("A33412 (F, N) - Encoder 3: Encoder signals an internal alarm");
            break;
        case 33414:
            /* A33414 (F, N) - Encoder 3: Signal level track C or D out of tolerance */
            return QObject::tr("A33414 (F, N) - Encoder 3: Signal level track C or D out of tolerance");
            break;
        case 33415:
            /* N33415 (F, A) - Encoder 3: Signal level track A or B outside tolerance (alarm) */
            return QObject::tr("N33415 (F, A) - Encoder 3: Signal level track A or B outside tolerance (alarm)");
            break;
        case 33418:
            /* A33418 (F, N) - Encoder 3: Speed change not plausible (alarm) */
            return QObject::tr("A33418 (F, N) - Encoder 3: Speed change not plausible (alarm)");
            break;
        case 33419:
            /* A33419 (F, N) - Encoder 3: Track A or B outside tolerance */
            return QObject::tr("A33419 (F, N) - Encoder 3: Track A or B outside tolerance");
            break;
        case 33421:
            /* A33421 (F, N) - Encoder 3: Determined commutation position incorrect (alarm) */
            return QObject::tr("A33421 (F, N) - Encoder 3: Determined commutation position incorrect (alarm)");
            break;
        case 33422:
            /* A33422 (F, N) - Encoder 3: Pulses per revolution square-wave encoder outside tolerance bandwidth */
            return QObject::tr("A33422 (F, N) - Encoder 3: Pulses per revolution square-wave encoder outside tolerance bandwidth");
            break;
        case 33429:
            /* A33429 (F, N) - Encoder 3: Position difference hall sensor/track C/D and A/B too large */
            return QObject::tr("A33429 (F, N) - Encoder 3: Position difference hall sensor/track C/D and A/B too large");
            break;
        case 33431:
            /* A33431 (F, N) - Encoder 3: Position deviation incremental/absolute too high (alarm) */
            return QObject::tr("A33431 (F, N) - Encoder 3: Position deviation incremental/absolute too high (alarm)");
            break;
        case 33432:
            /* A33432 (F, N) - Encoder 3: Rotor position adaptation corrects deviation */
            return QObject::tr("A33432 (F, N) - Encoder 3: Rotor position adaptation corrects deviation");
            break;
        case 33442:
            /* A33442 (F, N) - Encoder 3: Battery voltage alarm threshold reached */
            return QObject::tr("A33442 (F, N) - Encoder 3: Battery voltage alarm threshold reached");
            break;
        case 33443:
            /* A33443 (F, N) - Encoder 3: Signal level track C/D outside tolerance (alarm) */
            return QObject::tr("A33443 (F, N) - Encoder 3: Signal level track C/D outside tolerance (alarm)");
            break;
        case 33460:
            /* A33460 (N) - Encoder 3: Analog sensor channel A failed */
            return QObject::tr("A33460 (N) - Encoder 3: Analog sensor channel A failed");
            break;
        case 33461:
            /* A33461 (N) - Encoder 3: Analog sensor channel B failed */
            return QObject::tr("A33461 (N) - Encoder 3: Analog sensor channel B failed");
            break;
        case 33462:
            /* A33462 (N) - Encoder 3: Analog sensor no channel active */
            return QObject::tr("A33462 (N) - Encoder 3: Analog sensor no channel active");
            break;
        case 33463:
            /* A33463 (N) - Encoder 3: Analog sensor position value exceeds limit value */
            return QObject::tr("A33463 (N) - Encoder 3: Analog sensor position value exceeds limit value");
            break;
        case 33470:
            /* A33470 (F, N) - Encoder 3: Encoder signals an internal error (X521.7) */
            return QObject::tr("A33470 (F, N) - Encoder 3: Encoder signals an internal error (X521.7)");
            break;
        case 33500:
            /* F33500 (N, A) - Encoder 3: Position tracking traversing range exceeded */
            return QObject::tr("F33500 (N, A) - Encoder 3: Position tracking traversing range exceeded");
            break;
        case 33501:
            /* F33501 (N, A) - Encoder 3: Position tracking encoder position outside tolerance window */
            return QObject::tr("F33501 (N, A) - Encoder 3: Position tracking encoder position outside tolerance window");
            break;
        case 33502:
            /* F33502 (N, A) - Encoder 3: Encoder with measuring gear without valid signals */
            return QObject::tr("F33502 (N, A) - Encoder 3: Encoder with measuring gear without valid signals");
            break;
        case 33503:
            /* F33503 (N, A) - Encoder 3: Position tracking cannot be reset */
            return QObject::tr("F33503 (N, A) - Encoder 3: Position tracking cannot be reset");
            break;
        case 33700:
            /* A33700 - Encoder 3: Effectivity test does not supply the expected value */
            return QObject::tr("A33700 - Encoder 3: Effectivity test does not supply the expected value");
            break;
        case 33800:
            /* N33800 (F) - Encoder 3: Group signal */
            return QObject::tr("N33800 (F) - Encoder 3: Group signal");
            break;
        case 33801:
            /* F33801 (N, A) - Encoder 3 DRIVE-CLiQ: Sign-of-life missing */
            return QObject::tr("F33801 (N, A) - Encoder 3 DRIVE-CLiQ: Sign-of-life missing");
            break;
        case 33802:
            /* F33802 (N, A) - Encoder 3: Time slice overflow */
            return QObject::tr("F33802 (N, A) - Encoder 3: Time slice overflow");
            break;
        case 33804:
            /* F33804 (N, A) - Encoder 3: Sensor Module checksum error */
            return QObject::tr("F33804 (N, A) - Encoder 3: Sensor Module checksum error");
            break;
        case 33805:
            /* F33805 (N, A) - Encoder 3: EEPROM checksum error */
            return QObject::tr("F33805 (N, A) - Encoder 3: EEPROM checksum error");
            break;
        case 33806:
            /* F33806 (N, A) - Encoder 3: Initialization error */
            return QObject::tr("F33806 (N, A) - Encoder 3: Initialization error");
            break;
        case 33811:
            /* A33811 (F, N) - Encoder 3: Encoder serial number changed */
            return QObject::tr("A33811 (F, N) - Encoder 3: Encoder serial number changed");
            break;
        case 33812:
            /* F33812 (N, A) - Encoder 3: Requested cycle or RX-/TX timing not supported */
            return QObject::tr("F33812 (N, A) - Encoder 3: Requested cycle or RX-/TX timing not supported");
            break;
        case 33813:
            /* F33813 - Encoder 3: Hardware logic unit failed */
            return QObject::tr("F33813 - Encoder 3: Hardware logic unit failed");
            break;
        case 33820:
            /* F33820 (N, A) - Encoder 3 DRIVE-CLiQ: Telegram error */
            return QObject::tr("F33820 (N, A) - Encoder 3 DRIVE-CLiQ: Telegram error");
            break;
        case 33835:
            /* F33835 (N, A) - Encoder 3 DRIVE-CLiQ: Cyclic data transfer error */
            return QObject::tr("F33835 (N, A) - Encoder 3 DRIVE-CLiQ: Cyclic data transfer error");
            break;
        case 33836:
            /* F33836 (N, A) - Encoder 3 DRIVE-CLiQ: Send error for DRIVE-CLiQ data */
            return QObject::tr("F33836 (N, A) - Encoder 3 DRIVE-CLiQ: Send error for DRIVE-CLiQ data");
            break;
        case 33837:
            /* F33837 (N, A) - Encoder 3 DRIVE-CLiQ: Component fault */
            return QObject::tr("F33837 (N, A) - Encoder 3 DRIVE-CLiQ: Component fault");
            break;
        case 33845:
            /* F33845 (N, A) - Encoder 3 DRIVE-CLiQ: Cyclic data transfer error */
            return QObject::tr("F33845 (N, A) - Encoder 3 DRIVE-CLiQ: Cyclic data transfer error");
            break;
        case 33850:
            /* F33850 (N, A) - Encoder 3: Encoder evaluation internal software error */
            return QObject::tr("F33850 (N, A) - Encoder 3: Encoder evaluation internal software error");
            break;
        case 33851:
            /* F33851 (N, A) - Encoder 3 DRIVE-CLiQ (CU): Sign-of-life missing */
            return QObject::tr("F33851 (N, A) - Encoder 3 DRIVE-CLiQ (CU): Sign-of-life missing");
            break;
        case 33860:
            /* F33860 (N, A) - Encoder 3 DRIVE-CLiQ (CU): Telegram error */
            return QObject::tr("F33860 (N, A) - Encoder 3 DRIVE-CLiQ (CU): Telegram error");
            break;
        case 33875:
            /* F33875 (N, A) - Encoder 3: power supply voltage failed */
            return QObject::tr("F33875 (N, A) - Encoder 3: power supply voltage failed");
            break;
        case 33885:
            /* F33885 (N, A) - Encoder 3 DRIVE-CLiQ (CU): Cyclic data transfer error */
            return QObject::tr("F33885 (N, A) - Encoder 3 DRIVE-CLiQ (CU): Cyclic data transfer error");
            break;
        case 33886:
            /* F33886 (N, A) - Encoder 3 DRIVE-CLiQ (CU): Error when sending DRIVE-CLiQ data */
            return QObject::tr("F33886 (N, A) - Encoder 3 DRIVE-CLiQ (CU): Error when sending DRIVE-CLiQ data");
            break;
        case 33887:
            /* F33887 (N, A) - Encoder 3 DRIVE-CLiQ (CU): Component fault */
            return QObject::tr("F33887 (N, A) - Encoder 3 DRIVE-CLiQ (CU): Component fault");
            break;
        case 33895:
            /* F33895 (N, A) - Encoder 3 DRIVE-CLiQ (CU): Alternating cyclic data transfer error */
            return QObject::tr("F33895 (N, A) - Encoder 3 DRIVE-CLiQ (CU): Alternating cyclic data transfer error");
            break;
        case 33896:
            /* F33896 (N, A) - Encoder 3 DRIVE-CLiQ (CU): Inconsistent component properties */
            return QObject::tr("F33896 (N, A) - Encoder 3 DRIVE-CLiQ (CU): Inconsistent component properties");
            break;
        case 33899:
            /* F33899 (N, A) - Encoder 3: Unknown fault */
            return QObject::tr("F33899 (N, A) - Encoder 3: Unknown fault");
            break;
        case 33902:
            /* A33902 (F, N) - Encoder 3: SPI-BUS error occurred */
            return QObject::tr("A33902 (F, N) - Encoder 3: SPI-BUS error occurred");
            break;
        case 33903:
            /* A33903 (F, N) - Encoder 3: I2C-BUS error occurred */
            return QObject::tr("A33903 (F, N) - Encoder 3: I2C-BUS error occurred");
            break;
        case 33905:
            /* F33905 (N, A) - Encoder 3: Encoder parameterization error */
            return QObject::tr("F33905 (N, A) - Encoder 3: Encoder parameterization error");
            break;
        case 33912:
            /* F33912 - Encoder 3: Device combination is not permissible */
            return QObject::tr("F33912 - Encoder 3: Device combination is not permissible");
            break;
        case 33915:
            /* A33915 (F, N) - Encoder 3: Encoder configuration error */
            return QObject::tr("A33915 (F, N) - Encoder 3: Encoder configuration error");
            break;
        case 33916:
            /* F33916 (N, A) - Encoder 3: Encoder parameterization error */
            return QObject::tr("F33916 (N, A) - Encoder 3: Encoder parameterization error");
            break;
        case 33920:
            /* A33920 (F, N) - Encoder 3: Temperature sensor fault (motor) */
            return QObject::tr("A33920 (F, N) - Encoder 3: Temperature sensor fault (motor)");
            break;
        case 33930:
            /* A33930 (N) - Encoder 3: Data logger has saved data */
            return QObject::tr("A33930 (N) - Encoder 3: Data logger has saved data");
            break;
        case 33940:
            /* A33940 (F, N) - Encoder 3: Spindle sensor S1 voltage incorrect */
            return QObject::tr("A33940 (F, N) - Encoder 3: Spindle sensor S1 voltage incorrect");
            break;
        case 33950:
            /* F33950 - Encoder 3: Internal software error */
            return QObject::tr("F33950 - Encoder 3: Internal software error");
            break;
        case 33999:
            /* A33999 (F, N) - Encoder 3: Unknown alarm */
            return QObject::tr("A33999 (F, N) - Encoder 3: Unknown alarm");
            break;
        case 34207:
            /* F34207 (N, A) - VSM: Temperature fault threshold exceeded */
            return QObject::tr("F34207 (N, A) - VSM: Temperature fault threshold exceeded");
            break;
        case 34211:
            /* A34211 (F, N) - VSM: Temperature alarm threshold exceeded */
            return QObject::tr("A34211 (F, N) - VSM: Temperature alarm threshold exceeded");
            break;
        case 34800:
            /* N34800 (F) - VSM: Group signal */
            return QObject::tr("N34800 (F) - VSM: Group signal");
            break;
        case 34801:
            /* F34801 (N, A) - VSM DRIVE-CLiQ: Sign-of-life missing */
            return QObject::tr("F34801 (N, A) - VSM DRIVE-CLiQ: Sign-of-life missing");
            break;
        case 34802:
            /* F34802 (N, A) - VSM: Time slice overflow */
            return QObject::tr("F34802 (N, A) - VSM: Time slice overflow");
            break;
        case 34803:
            /* F34803 - VSM: Memory test */
            return QObject::tr("F34803 - VSM: Memory test");
            break;
        case 34804:
            /* F34804 (N, A) - VSM: CRC */
            return QObject::tr("F34804 (N, A) - VSM: CRC");
            break;
        case 34805:
            /* F34805 (N, A) - VSM: EEPROM checksum error */
            return QObject::tr("F34805 (N, A) - VSM: EEPROM checksum error");
            break;
        case 34806:
            /* F34806 - VSM: Initialization */
            return QObject::tr("F34806 - VSM: Initialization");
            break;
        case 34807:
            /* A34807 (F, N) - VSM: Sequence control time monitoring */
            return QObject::tr("A34807 (F, N) - VSM: Sequence control time monitoring");
            break;
        case 34820:
            /* F34820 - VSM DRIVE-CLiQ: Telegram error */
            return QObject::tr("F34820 - VSM DRIVE-CLiQ: Telegram error");
            break;
        case 34835:
            /* F34835 - VSM DRIVE-CLiQ: Cyclic data transfer error */
            return QObject::tr("F34835 - VSM DRIVE-CLiQ: Cyclic data transfer error");
            break;
        case 34836:
            /* F34836 - VSM DRIVE-CLiQ: Send error for DRIVE-CLiQ data */
            return QObject::tr("F34836 - VSM DRIVE-CLiQ: Send error for DRIVE-CLiQ data");
            break;
        case 34837:
            /* F34837 - VSM DRIVE-CLiQ: Component fault */
            return QObject::tr("F34837 - VSM DRIVE-CLiQ: Component fault");
            break;
        case 34845:
            /* F34845 - VSM DRIVE-CLiQ: Cyclic data transfer error */
            return QObject::tr("F34845 - VSM DRIVE-CLiQ: Cyclic data transfer error");
            break;
        case 34850:
            /* F34850 - VSM: Internal software error */
            return QObject::tr("F34850 - VSM: Internal software error");
            break;
        case 34851:
            /* F34851 - VSM DRIVE-CLiQ (CU): Sign-of-life missing */
            return QObject::tr("F34851 - VSM DRIVE-CLiQ (CU): Sign-of-life missing");
            break;
        case 34860:
            /* F34860 - VSM DRIVE-CLiQ (CU): Telegram error */
            return QObject::tr("F34860 - VSM DRIVE-CLiQ (CU): Telegram error");
            break;
        case 34875:
            /* F34875 - VSM: power supply voltage failed */
            return QObject::tr("F34875 - VSM: power supply voltage failed");
            break;
        case 34885:
            /* F34885 - VSM DRIVE-CLiQ (CU): Cyclic data transfer error */
            return QObject::tr("F34885 - VSM DRIVE-CLiQ (CU): Cyclic data transfer error");
            break;
        case 34886:
            /* F34886 - VSM DRIVE-CLiQ (CU): Error when sending DRIVE-CLiQ data */
            return QObject::tr("F34886 - VSM DRIVE-CLiQ (CU): Error when sending DRIVE-CLiQ data");
            break;
        case 34887:
            /* F34887 - VSM DRIVE-CLiQ (CU): Component fault */
            return QObject::tr("F34887 - VSM DRIVE-CLiQ (CU): Component fault");
            break;
        case 34895:
            /* F34895 - VSM DRIVE-CLiQ (CU): Alternating cyclic data transfer error */
            return QObject::tr("F34895 - VSM DRIVE-CLiQ (CU): Alternating cyclic data transfer error");
            break;
        case 34896:
            /* F34896 - VSM DRIVE-CLiQ (CU): Inconsistent component properties */
            return QObject::tr("F34896 - VSM DRIVE-CLiQ (CU): Inconsistent component properties");
            break;
        case 34899:
            /* F34899 (N, A) - VSM: Unknown fault */
            return QObject::tr("F34899 (N, A) - VSM: Unknown fault");
            break;
        case 34903:
            /* A34903 (F, N) - VSM: I2C bus error occurred */
            return QObject::tr("A34903 (F, N) - VSM: I2C bus error occurred");
            break;
        case 34904:
            /* A34904 (F, N) - VSM: EEPROM */
            return QObject::tr("A34904 (F, N) - VSM: EEPROM");
            break;
        case 34905:
            /* A34905 (F, N) - VSM: Parameter access */
            return QObject::tr("A34905 (F, N) - VSM: Parameter access");
            break;
        case 34920:
            /* F34920 (N, A) - VSM: overtemperature or temperature sensor fault */
            return QObject::tr("F34920 (N, A) - VSM: overtemperature or temperature sensor fault");
            break;
        case 34950:
            /* F34950 - VSM: Internal software error */
            return QObject::tr("F34950 - VSM: Internal software error");
            break;
        case 34999:
            /* A34999 (F, N) - VSM: Unknown alarm */
            return QObject::tr("A34999 (F, N) - VSM: Unknown alarm");
            break;
        case 35000:
            /* F35000 - TM54F: Sampling time invalid */
            return QObject::tr("F35000 - TM54F: Sampling time invalid");
            break;
        case 35001:
            /* F35001 - TM54F: Parameter value invalid */
            return QObject::tr("F35001 - TM54F: Parameter value invalid");
            break;
        case 35002:
            /* F35002 - TM54F: Commissioning not possible */
            return QObject::tr("F35002 - TM54F: Commissioning not possible");
            break;
        case 35003:
            /* F35003 - TM54F: Acknowledgment on the Control Unit is required */
            return QObject::tr("F35003 - TM54F: Acknowledgment on the Control Unit is required");
            break;
        case 35004:
            /* F35004 - TM54F: communication cycle invalid */
            return QObject::tr("F35004 - TM54F: communication cycle invalid");
            break;
        case 35005:
            /* F35005 - TM54F:parallel connection not supported */
            return QObject::tr("F35005 - TM54F:parallel connection not supported");
            break;
        case 35006:
            /* F35006 - TM54F: drive groups invalid */
            return QObject::tr("F35006 - TM54F: drive groups invalid");
            break;
        case 35009:
            /* F35009 - TM54F: Safety commissioning drive incomplete */
            return QObject::tr("F35009 - TM54F: Safety commissioning drive incomplete");
            break;
        case 35011:
            /* F35011 - TM54F: Drive object number assignment illegal */
            return QObject::tr("F35011 - TM54F: Drive object number assignment illegal");
            break;
        case 35012:
            /* A35012 - TM54F: Test stop for failsafe digital inputs/outputs */
            return QObject::tr("A35012 - TM54F: Test stop for failsafe digital inputs/outputs");
            break;
        case 35013:
            /* F35013 - TM54F: Test stop error */
            return QObject::tr("F35013 - TM54F: Test stop error");
            break;
        case 35014:
            /* A35014 - TM54F: Test stop for failsafe digital inputs/outputs */
            return QObject::tr("A35014 - TM54F: Test stop for failsafe digital inputs/outputs");
            break;
        case 35015:
            /* A35015 - TM54F: Motor/Hydraulic Module replaced or configuration inconsistent */
            return QObject::tr("A35015 - TM54F: Motor/Hydraulic Module replaced or configuration inconsistent");
            break;
        case 35016:
            /* A35016 - TM54F: Net data communication with drive not established */
            return QObject::tr("A35016 - TM54F: Net data communication with drive not established");
            break;
        case 35040:
            /* F35040 - TM54F: 24 V undervoltage */
            return QObject::tr("F35040 - TM54F: 24 V undervoltage");
            break;
        case 35043:
            /* F35043 - TM54F: 24 V overvoltage */
            return QObject::tr("F35043 - TM54F: 24 V overvoltage");
            break;
        case 35051:
            /* F35051 - TM54F: Defect in a monitoring channel */
            return QObject::tr("F35051 - TM54F: Defect in a monitoring channel");
            break;
        case 35052:
            /* F35052 (A) - TM54F: Internal hardware error */
            return QObject::tr("F35052 (A) - TM54F: Internal hardware error");
            break;
        case 35053:
            /* F35053 - TM54F: Temperature fault threshold exceeded */
            return QObject::tr("F35053 - TM54F: Temperature fault threshold exceeded");
            break;
        case 35054:
            /* A35054 - TM54F: Temperature alarm threshold exceeded */
            return QObject::tr("A35054 - TM54F: Temperature alarm threshold exceeded");
            break;
        case 35075:
            /* A35075 (F) - TM54F: Error during internal communication */
            return QObject::tr("A35075 (F) - TM54F: Error during internal communication");
            break;
        case 35080:
            /* A35080 (F) - TM54F: Checksum error safety parameters */
            return QObject::tr("A35080 (F) - TM54F: Checksum error safety parameters");
            break;
        case 35081:
            /* A35081 (F) - TM54F: Static (steady state) 1 signal at the F-DI for safe acknowledgment */
            return QObject::tr("A35081 (F) - TM54F: Static (steady state) 1 signal at the F-DI for safe acknowledgment");
            break;
        case 35150:
            /* F35150 - TM54F: Communication error */
            return QObject::tr("F35150 - TM54F: Communication error");
            break;
        case 35151:
            /* F35151 - TM54F: Discrepancy error */
            return QObject::tr("F35151 - TM54F: Discrepancy error");
            break;
        case 35152:
            /* F35152 - TM54F: Internal software error */
            return QObject::tr("F35152 - TM54F: Internal software error");
            break;
        case 35200:
            /* A35200 (F, N) - TM: Calibration data */
            return QObject::tr("A35200 (F, N) - TM: Calibration data");
            break;
        case 35207:
            /* F35207 (N, A) - TM: Temperature fault/alarm threshold channel 0 exceeded */
            return QObject::tr("F35207 (N, A) - TM: Temperature fault/alarm threshold channel 0 exceeded");
            break;
        case 35208:
            /* F35208 (N, A) - TM: Temperature fault/alarm threshold channel 1 exceeded */
            return QObject::tr("F35208 (N, A) - TM: Temperature fault/alarm threshold channel 1 exceeded");
            break;
        case 35209:
            /* F35209 (N, A) - TM: Temperature fault/alarm threshold channel 2 exceeded */
            return QObject::tr("F35209 (N, A) - TM: Temperature fault/alarm threshold channel 2 exceeded");
            break;
        case 35210:
            /* F35210 (N, A) - TM: Temperature fault/alarm threshold channel 3 exceeded */
            return QObject::tr("F35210 (N, A) - TM: Temperature fault/alarm threshold channel 3 exceeded");
            break;
        case 35211:
            /* A35211 (F, N) - TM: Temperature alarm threshold channel 0 exceeded */
            return QObject::tr("A35211 (F, N) - TM: Temperature alarm threshold channel 0 exceeded");
            break;
        case 35212:
            /* A35212 (F, N) - TM: Temperature alarm threshold channel 1 exceeded */
            return QObject::tr("A35212 (F, N) - TM: Temperature alarm threshold channel 1 exceeded");
            break;
        case 35213:
            /* A35213 (F, N) - TM: Temperature alarm threshold channel 2 exceeded */
            return QObject::tr("A35213 (F, N) - TM: Temperature alarm threshold channel 2 exceeded");
            break;
        case 35214:
            /* A35214 (F, N) - TM: Temperature alarm threshold channel 3 exceeded */
            return QObject::tr("A35214 (F, N) - TM: Temperature alarm threshold channel 3 exceeded");
            break;
        case 35220:
            /* F35220 (N, A) - TM: Frequency limit reached for signal output */
            return QObject::tr("F35220 (N, A) - TM: Frequency limit reached for signal output");
            break;
        case 35221:
            /* F35221 (N, A) - TM: Setpoint - actual value deviation outside the tolerance range */
            return QObject::tr("F35221 (N, A) - TM: Setpoint - actual value deviation outside the tolerance range");
            break;
        case 35222:
            /* A35222 (F, N) - TM: Encoder pulse number not permissible */
            return QObject::tr("A35222 (F, N) - TM: Encoder pulse number not permissible");
            break;
        case 35223:
            /* A35223 (F, N) - TM: Zero mark offset not permissible */
            return QObject::tr("A35223 (F, N) - TM: Zero mark offset not permissible");
            break;
        case 35224:
            /* A35224 (N) - TM: Zero mark synchronization interrupted */
            return QObject::tr("A35224 (N) - TM: Zero mark synchronization interrupted");
            break;
        case 35225:
            /* A35225 - TM: Zero mark synchronization held - encoder not in the ready state */
            return QObject::tr("A35225 - TM: Zero mark synchronization held - encoder not in the ready state");
            break;
        case 35226:
            /* A35226 - TM: Tracks A/B are deactivated */
            return QObject::tr("A35226 - TM: Tracks A/B are deactivated");
            break;
        case 35227:
            /* A35227 - EDS changeover/encoder data set change not supported */
            return QObject::tr("A35227 - EDS changeover/encoder data set change not supported");
            break;
        case 35228:
            /* A35228 (F, N) - TM: Sampling time p4099[3] invalid */
            return QObject::tr("A35228 (F, N) - TM: Sampling time p4099[3] invalid");
            break;
        case 35229:
            /* F35229 - TM: Time slice deactivated */
            return QObject::tr("F35229 - TM: Time slice deactivated");
            break;
        case 35230:
            /* F35230 - TM: Hardware fault */
            return QObject::tr("F35230 - TM: Hardware fault");
            break;
        case 35231:
            /* A35231 - TM: Master control by PLC missing */
            return QObject::tr("A35231 - TM: Master control by PLC missing");
            break;
        case 35232:
            /* A35232 - TM41: Zero mark no longer synchronous POWER ON required */
            return QObject::tr("A35232 - TM41: Zero mark no longer synchronous POWER ON required");
            break;
        case 35233:
            /* F35233 - DRIVE-CLiQ component function not supported */
            return QObject::tr("F35233 - DRIVE-CLiQ component function not supported");
            break;
        case 35400:
            /* F35400 (N, A) - TM: Temperature fault/alarm threshold channel 4 exceeded */
            return QObject::tr("F35400 (N, A) - TM: Temperature fault/alarm threshold channel 4 exceeded");
            break;
        case 35401:
            /* F35401 (N, A) - TM: Temperature fault/alarm threshold channel 5 exceeded */
            return QObject::tr("F35401 (N, A) - TM: Temperature fault/alarm threshold channel 5 exceeded");
            break;
        case 35402:
            /* F35402 (N, A) - TM: Temperature fault/alarm threshold channel 6 exceeded */
            return QObject::tr("F35402 (N, A) - TM: Temperature fault/alarm threshold channel 6 exceeded");
            break;
        case 35403:
            /* F35403 (N, A) - TM: Temperature fault/alarm threshold channel 7 exceeded */
            return QObject::tr("F35403 (N, A) - TM: Temperature fault/alarm threshold channel 7 exceeded");
            break;
        case 35404:
            /* F35404 (N, A) - TM: Temperature fault/alarm threshold channel 8 exceeded */
            return QObject::tr("F35404 (N, A) - TM: Temperature fault/alarm threshold channel 8 exceeded");
            break;
        case 35405:
            /* F35405 (N, A) - TM: Temperature fault/alarm threshold channel 9 exceeded */
            return QObject::tr("F35405 (N, A) - TM: Temperature fault/alarm threshold channel 9 exceeded");
            break;
        case 35406:
            /* F35406 (N, A) - TM: Temperature fault/alarm threshold channel 10 exceeded */
            return QObject::tr("F35406 (N, A) - TM: Temperature fault/alarm threshold channel 10 exceeded");
            break;
        case 35407:
            /* F35407 (N, A) - TM: Temperature fault/alarm threshold channel 11 exceeded */
            return QObject::tr("F35407 (N, A) - TM: Temperature fault/alarm threshold channel 11 exceeded");
            break;
        case 35410:
            /* A35410 (F, N) - TM: Temperature alarm threshold channel 4 exceeded */
            return QObject::tr("A35410 (F, N) - TM: Temperature alarm threshold channel 4 exceeded");
            break;
        case 35411:
            /* A35411 (F, N) - TM: Temperature alarm threshold channel 5 exceeded */
            return QObject::tr("A35411 (F, N) - TM: Temperature alarm threshold channel 5 exceeded");
            break;
        case 35412:
            /* A35412 (F, N) - TM: Temperature alarm threshold channel 6 exceeded */
            return QObject::tr("A35412 (F, N) - TM: Temperature alarm threshold channel 6 exceeded");
            break;
        case 35413:
            /* A35413 (F, N) - TM: Temperature alarm threshold channel 7 exceeded */
            return QObject::tr("A35413 (F, N) - TM: Temperature alarm threshold channel 7 exceeded");
            break;
        case 35414:
            /* A35414 (F, N) - TM: Temperature alarm threshold channel 8 exceeded */
            return QObject::tr("A35414 (F, N) - TM: Temperature alarm threshold channel 8 exceeded");
            break;
        case 35415:
            /* A35415 (F, N) - TM: Temperature alarm threshold channel 9 exceeded */
            return QObject::tr("A35415 (F, N) - TM: Temperature alarm threshold channel 9 exceeded");
            break;
        case 35416:
            /* A35416 (F, N) - TM: Temperature alarm threshold channel 10 exceeded */
            return QObject::tr("A35416 (F, N) - TM: Temperature alarm threshold channel 10 exceeded");
            break;
        case 35417:
            /* A35417 (F, N) - TM: Temperature alarm threshold channel 11 exceeded */
            return QObject::tr("A35417 (F, N) - TM: Temperature alarm threshold channel 11 exceeded");
            break;
        case 35800:
            /* N35800 (F) - TM: Group signal */
            return QObject::tr("N35800 (F) - TM: Group signal");
            break;
        case 35801:
            /* F35801 (N, A) - TM DRIVE-CLiQ: Sign-of-life missing */
            return QObject::tr("F35801 (N, A) - TM DRIVE-CLiQ: Sign-of-life missing");
            break;
        case 35802:
            /* A35802 (F, N) - TM: Time slice overflow */
            return QObject::tr("A35802 (F, N) - TM: Time slice overflow");
            break;
        case 35803:
            /* A35803 (F, N) - TM: Memory test */
            return QObject::tr("A35803 (F, N) - TM: Memory test");
            break;
        case 35804:
            /* F35804 (N, A) - TM: CRC */
            return QObject::tr("F35804 (N, A) - TM: CRC");
            break;
        case 35805:
            /* F35805 (N, A) - TM: EEPROM checksum error */
            return QObject::tr("F35805 (N, A) - TM: EEPROM checksum error");
            break;
        case 35807:
            /* A35807 (F, N) - TM: Sequence control time monitoring */
            return QObject::tr("A35807 (F, N) - TM: Sequence control time monitoring");
            break;
        case 35820:
            /* F35820 - TM DRIVE-CLiQ: Telegram error */
            return QObject::tr("F35820 - TM DRIVE-CLiQ: Telegram error");
            break;
        case 35835:
            /* F35835 - TM DRIVE-CLiQ: Cyclic data transfer error */
            return QObject::tr("F35835 - TM DRIVE-CLiQ: Cyclic data transfer error");
            break;
        case 35836:
            /* F35836 - TM DRIVE-CLiQ: Send error for DRIVE-CLiQ data */
            return QObject::tr("F35836 - TM DRIVE-CLiQ: Send error for DRIVE-CLiQ data");
            break;
        case 35837:
            /* F35837 - PTM DRIVE-CLiQ: Component fault */
            return QObject::tr("F35837 - PTM DRIVE-CLiQ: Component fault");
            break;
        case 35845:
            /* F35845 - TM DRIVE-CLiQ: Cyclic data transfer error */
            return QObject::tr("F35845 - TM DRIVE-CLiQ: Cyclic data transfer error");
            break;
        case 35850:
            /* F35850 - TM: Internal software error */
            return QObject::tr("F35850 - TM: Internal software error");
            break;
        case 35851:
            /* F35851 - TM DRIVE-CLiQ (CU): Sign-of-life missing */
            return QObject::tr("F35851 - TM DRIVE-CLiQ (CU): Sign-of-life missing");
            break;
        case 35860:
            /* F35860 - TM DRIVE-CLiQ (CU): Telegram error */
            return QObject::tr("F35860 - TM DRIVE-CLiQ (CU): Telegram error");
            break;
        case 35875:
            /* F35875 - TM: power supply voltage failed */
            return QObject::tr("F35875 - TM: power supply voltage failed");
            break;
        case 35885:
            /* F35885 - TM DRIVE-CLiQ (CU): Cyclic data transfer error */
            return QObject::tr("F35885 - TM DRIVE-CLiQ (CU): Cyclic data transfer error");
            break;
        case 35886:
            /* F35886 - TM DRIVE-CLiQ (CU): Error when sending DRIVE-CLiQ data */
            return QObject::tr("F35886 - TM DRIVE-CLiQ (CU): Error when sending DRIVE-CLiQ data");
            break;
        case 35887:
            /* F35887 - TM DRIVE-CLiQ (CU): Component fault */
            return QObject::tr("F35887 - TM DRIVE-CLiQ (CU): Component fault");
            break;
        case 35895:
            /* F35895 - TM DRIVE-CLiQ (CU): Alternating cyclic data transfer error */
            return QObject::tr("F35895 - TM DRIVE-CLiQ (CU): Alternating cyclic data transfer error");
            break;
        case 35896:
            /* F35896 - TM DRIVE-CLiQ (CU): Inconsistent component properties */
            return QObject::tr("F35896 - TM DRIVE-CLiQ (CU): Inconsistent component properties");
            break;
        case 35899:
            /* F35899 (N, A) - TM: Unknown fault */
            return QObject::tr("F35899 (N, A) - TM: Unknown fault");
            break;
        case 35903:
            /* A35903 (F, N) - TM: I2C bus error occurred */
            return QObject::tr("A35903 (F, N) - TM: I2C bus error occurred");
            break;
        case 35904:
            /* A35904 (F, N) - TM: EEPROM */
            return QObject::tr("A35904 (F, N) - TM: EEPROM");
            break;
        case 35905:
            /* A35905 (F, N) - TM: Parameter access */
            return QObject::tr("A35905 (F, N) - TM: Parameter access");
            break;
        case 35906:
            /* A35906 (F, N) - TM: 24 V power supply missing */
            return QObject::tr("A35906 (F, N) - TM: 24 V power supply missing");
            break;
        case 35907:
            /* A35907 (F, N) - TM: Hardware initialization error */
            return QObject::tr("A35907 (F, N) - TM: Hardware initialization error");
            break;
        case 35910:
            /* A35910 (F, N) - TM: Module overtemperature */
            return QObject::tr("A35910 (F, N) - TM: Module overtemperature");
            break;
        case 35911:
            /* A35911 (F, N) - TM: Clock synchronous operation sign-of-life missing */
            return QObject::tr("A35911 (F, N) - TM: Clock synchronous operation sign-of-life missing");
            break;
        case 35920:
            /* A35920 (F, N) - TM: Error temperature sensor channel 0 */
            return QObject::tr("A35920 (F, N) - TM: Error temperature sensor channel 0");
            break;
        case 35921:
            /* A35921 (F, N) - TM: Error temperature sensor channel 1 */
            return QObject::tr("A35921 (F, N) - TM: Error temperature sensor channel 1");
            break;
        case 35922:
            /* A35922 (F, N) - TM: Error temperature sensor channel 2 */
            return QObject::tr("A35922 (F, N) - TM: Error temperature sensor channel 2");
            break;
        case 35923:
            /* A35923 (F, N) - TM: Error temperature sensor channel 3 */
            return QObject::tr("A35923 (F, N) - TM: Error temperature sensor channel 3");
            break;
        case 35924:
            /* A35924 (F, N) - TM: Error temperature sensor channel 4 */
            return QObject::tr("A35924 (F, N) - TM: Error temperature sensor channel 4");
            break;
        case 35925:
            /* A35925 (F, N) - TM: Error temperature sensor channel 5 */
            return QObject::tr("A35925 (F, N) - TM: Error temperature sensor channel 5");
            break;
        case 35926:
            /* A35926 (F, N) - TM: Error temperature sensor channel 6 */
            return QObject::tr("A35926 (F, N) - TM: Error temperature sensor channel 6");
            break;
        case 35927:
            /* A35927 (F, N) - TM: Error temperature sensor channel 7 */
            return QObject::tr("A35927 (F, N) - TM: Error temperature sensor channel 7");
            break;
        case 35928:
            /* A35928 (F, N) - TM: Error temperature sensor channel 8 */
            return QObject::tr("A35928 (F, N) - TM: Error temperature sensor channel 8");
            break;
        case 35929:
            /* A35929 (F, N) - TM: Error temperature sensor channel 9 */
            return QObject::tr("A35929 (F, N) - TM: Error temperature sensor channel 9");
            break;
        case 35930:
            /* A35930 (F, N) - TM: Error temperature sensor channel 10 */
            return QObject::tr("A35930 (F, N) - TM: Error temperature sensor channel 10");
            break;
        case 35931:
            /* A35931 (F, N) - TM: Error temperature sensor channel 11 */
            return QObject::tr("A35931 (F, N) - TM: Error temperature sensor channel 11");
            break;
        case 35950:
            /* F35950 - TM: Internal software error */
            return QObject::tr("F35950 - TM: Internal software error");
            break;
        case 35999:
            /* A35999 (F, N) - TM: Unknown alarm */
            return QObject::tr("A35999 (F, N) - TM: Unknown alarm");
            break;
        case 36207:
            /* F36207 (N, A) - Hub: Overtemperature component */
            return QObject::tr("F36207 (N, A) - Hub: Overtemperature component");
            break;
        case 36211:
            /* A36211 (F, N) - Hub: Overtemperature alarm component */
            return QObject::tr("A36211 (F, N) - Hub: Overtemperature alarm component");
            break;
        case 36214:
            /* F36214 (N, A) - Hub: overvoltage fault 24 V supply */
            return QObject::tr("F36214 (N, A) - Hub: overvoltage fault 24 V supply");
            break;
        case 36216:
            /* F36216 (N, A) - Hub: undervoltage fault 24 V supply */
            return QObject::tr("F36216 (N, A) - Hub: undervoltage fault 24 V supply");
            break;
        case 36217:
            /* A36217 (N) - Hub: undervoltage alarm 24 V supply */
            return QObject::tr("A36217 (N) - Hub: undervoltage alarm 24 V supply");
            break;
        case 36800:
            /* N36800 (F) - Hub: Group signal */
            return QObject::tr("N36800 (F) - Hub: Group signal");
            break;
        case 36801:
            /* A36801 (F, N) - Hub DRIVE-CLiQ: Sign-of-life missing */
            return QObject::tr("A36801 (F, N) - Hub DRIVE-CLiQ: Sign-of-life missing");
            break;
        case 36802:
            /* A36802 (F, N) - Hub: Time slice overflow */
            return QObject::tr("A36802 (F, N) - Hub: Time slice overflow");
            break;
        case 36804:
            /* F36804 (N, A) - Hub: Checksum error */
            return QObject::tr("F36804 (N, A) - Hub: Checksum error");
            break;
        case 36805:
            /* F36805 (N, A) - Hub: EEPROM checksum incorrect */
            return QObject::tr("F36805 (N, A) - Hub: EEPROM checksum incorrect");
            break;
        case 36820:
            /* F36820 - Hub DRIVE-CLiQ: Telegram error */
            return QObject::tr("F36820 - Hub DRIVE-CLiQ: Telegram error");
            break;
        case 36835:
            /* F36835 - Hub DRIVE-CLiQ: Cyclic data transfer error */
            return QObject::tr("F36835 - Hub DRIVE-CLiQ: Cyclic data transfer error");
            break;
        case 36836:
            /* F36836 - Hub DRIVE-CLiQ: Send error for DRIVE-CLiQ data */
            return QObject::tr("F36836 - Hub DRIVE-CLiQ: Send error for DRIVE-CLiQ data");
            break;
        case 36837:
            /* F36837 - Hub DRIVE-CLiQ: Component fault */
            return QObject::tr("F36837 - Hub DRIVE-CLiQ: Component fault");
            break;
        case 36845:
            /* F36845 - Hub DRIVE-CLiQ: Cyclic data transfer error */
            return QObject::tr("F36845 - Hub DRIVE-CLiQ: Cyclic data transfer error");
            break;
        case 36851:
            /* F36851 - Hub DRIVE-CLiQ (CU): Sign-of-life missing */
            return QObject::tr("F36851 - Hub DRIVE-CLiQ (CU): Sign-of-life missing");
            break;
        case 36860:
            /* F36860 - Hub DRIVE-CLiQ (CU): Telegram error */
            return QObject::tr("F36860 - Hub DRIVE-CLiQ (CU): Telegram error");
            break;
        case 36875:
            /* F36875 - HUB: power supply voltage failed */
            return QObject::tr("F36875 - HUB: power supply voltage failed");
            break;
        case 36885:
            /* F36885 - Hub DRIVE-CLiQ (CU): Cyclic data transfer error */
            return QObject::tr("F36885 - Hub DRIVE-CLiQ (CU): Cyclic data transfer error");
            break;
        case 36886:
            /* F36886 - Hub DRIVE-CLiQ (CU): Error when sending DRIVE-CLiQ data */
            return QObject::tr("F36886 - Hub DRIVE-CLiQ (CU): Error when sending DRIVE-CLiQ data");
            break;
        case 36887:
            /* F36887 - Hub DRIVE-CLiQ (CU): Component fault */
            return QObject::tr("F36887 - Hub DRIVE-CLiQ (CU): Component fault");
            break;
        case 36895:
            /* F36895 - Hub DRIVE-CLiQ (CU): Alternating cyclic data transfer error */
            return QObject::tr("F36895 - Hub DRIVE-CLiQ (CU): Alternating cyclic data transfer error");
            break;
        case 36896:
            /* F36896 - Hub DRIVE-CLiQ (CU): Inconsistent component properties */
            return QObject::tr("F36896 - Hub DRIVE-CLiQ (CU): Inconsistent component properties");
            break;
        case 36899:
            /* F36899 (N, A) - Hub: Unknown fault */
            return QObject::tr("F36899 (N, A) - Hub: Unknown fault");
            break;
        case 36950:
            /* F36950 - Hub: Internal software error */
            return QObject::tr("F36950 - Hub: Internal software error");
            break;
        case 36999:
            /* A36999 (F, N) - Hub: Unknown alarm */
            return QObject::tr("A36999 (F, N) - Hub: Unknown alarm");
            break;
        case 37001:
            /* F37001 - HF Damping Module: overcurrent */
            return QObject::tr("F37001 - HF Damping Module: overcurrent");
            break;
        case 37002:
            /* F37002 - HF Damping Module: Damping voltage too high */
            return QObject::tr("F37002 - HF Damping Module: Damping voltage too high");
            break;
        case 37003:
            /* F37003 - HF Damping Module: Damping voltage not established */
            return QObject::tr("F37003 - HF Damping Module: Damping voltage not established");
            break;
        case 37004:
            /* F37004 - HF Damping Module: Heat sink overtemperature */
            return QObject::tr("F37004 - HF Damping Module: Heat sink overtemperature");
            break;
        case 37005:
            /* F37005 - HF Damping Module: I2t overload */
            return QObject::tr("F37005 - HF Damping Module: I2t overload");
            break;
        case 37012:
            /* F37012 - HF Damping Module: Heat sink temperature sensor wire breakage */
            return QObject::tr("F37012 - HF Damping Module: Heat sink temperature sensor wire breakage");
            break;
        case 37013:
            /* F37013 - HF Damping Module: Heat sink temperature sensor short-circuit */
            return QObject::tr("F37013 - HF Damping Module: Heat sink temperature sensor short-circuit");
            break;
        case 37024:
            /* F37024 - HF Damping Module: Overtemperature thermal model */
            return QObject::tr("F37024 - HF Damping Module: Overtemperature thermal model");
            break;
        case 37025:
            /* F37025 - HF Damping Module: Chip overtemperature */
            return QObject::tr("F37025 - HF Damping Module: Chip overtemperature");
            break;
        case 37034:
            /* A37034 - HF Damping Module: Internal overtemperature */
            return QObject::tr("A37034 - HF Damping Module: Internal overtemperature");
            break;
        case 37036:
            /* F37036 - HF Damping Module: Internal overtemperature */
            return QObject::tr("F37036 - HF Damping Module: Internal overtemperature");
            break;
        case 37040:
            /* F37040 - HF Damping Module: 24 V undervoltage */
            return QObject::tr("F37040 - HF Damping Module: 24 V undervoltage");
            break;
        case 37041:
            /* A37041 (F) - HF Damping Module: 24 V undervoltage alarm */
            return QObject::tr("A37041 (F) - HF Damping Module: 24 V undervoltage alarm");
            break;
        case 37043:
            /* F37043 - HF Damping Module: 24 V overvoltage */
            return QObject::tr("F37043 - HF Damping Module: 24 V overvoltage");
            break;
        case 37044:
            /* A37044 (F) - HF Damping Module: 24 V overvoltage alarm */
            return QObject::tr("A37044 (F) - HF Damping Module: 24 V overvoltage alarm");
            break;
        case 37045:
            /* F37045 - HF Damping Module: Supply undervoltage */
            return QObject::tr("F37045 - HF Damping Module: Supply undervoltage");
            break;
        case 37049:
            /* A37049 - HF Damping Module: Internal fan defective */
            return QObject::tr("A37049 - HF Damping Module: Internal fan defective");
            break;
        case 37050:
            /* F37050 - HF Damping Module: 24 V overvoltage fault */
            return QObject::tr("F37050 - HF Damping Module: 24 V overvoltage fault");
            break;
        case 37052:
            /* F37052 - HF Damping Module: EEPROM data error */
            return QObject::tr("F37052 - HF Damping Module: EEPROM data error");
            break;
        case 37056:
            /* A37056 (F) - HF Damping Module: Heat sink overtemperature */
            return QObject::tr("A37056 (F) - HF Damping Module: Heat sink overtemperature");
            break;
        case 37310:
            /* A37310 (F) - HF Choke Module: Overtemperature */
            return QObject::tr("A37310 (F) - HF Choke Module: Overtemperature");
            break;
        case 37311:
            /* F37311 - HF Choke Module: Heat sink overtemperature */
            return QObject::tr("F37311 - HF Choke Module: Heat sink overtemperature");
            break;
        case 37312:
            /* A37312 (F) - HF Choke Module: Overtemperature or fan failure */
            return QObject::tr("A37312 (F) - HF Choke Module: Overtemperature or fan failure");
            break;
        case 37313:
            /* F37313 - HF Choke Module: Overtemperature or fan failure */
            return QObject::tr("F37313 - HF Choke Module: Overtemperature or fan failure");
            break;
        case 37502:
            /* A37502 (F) - HF Damping Module: Damping voltage too high */
            return QObject::tr("A37502 (F) - HF Damping Module: Damping voltage too high");
            break;
        case 37800:
            /* N37800 (F) - HF Damping Module: Group signal */
            return QObject::tr("N37800 (F) - HF Damping Module: Group signal");
            break;
        case 37801:
            /* A37801 (F, N) - HF Damping Module: Sign-of-life missing */
            return QObject::tr("A37801 (F, N) - HF Damping Module: Sign-of-life missing");
            break;
        case 37802:
            /* F37802 (N, A) - HF Damping Module: time slice overflow */
            return QObject::tr("F37802 (N, A) - HF Damping Module: time slice overflow");
            break;
        case 37804:
            /* F37804 (N, A) - HF Damping Module: CRC */
            return QObject::tr("F37804 (N, A) - HF Damping Module: CRC");
            break;
        case 37805:
            /* F37805 - HF Damping Module: EEPROM checksum incorrect */
            return QObject::tr("F37805 - HF Damping Module: EEPROM checksum incorrect");
            break;
        case 37820:
            /* F37820 - HF Damping Module: Telegram error */
            return QObject::tr("F37820 - HF Damping Module: Telegram error");
            break;
        case 37835:
            /* F37835 - HF Damping Module: Cyclic data transmission error */
            return QObject::tr("F37835 - HF Damping Module: Cyclic data transmission error");
            break;
        case 37836:
            /* F37836 - HF Damping Module: Send error for DRIVE-CLiQ data */
            return QObject::tr("F37836 - HF Damping Module: Send error for DRIVE-CLiQ data");
            break;
        case 37837:
            /* F37837 - HF Damping Module: Component faulted */
            return QObject::tr("F37837 - HF Damping Module: Component faulted");
            break;
        case 37845:
            /* F37845 - HF Damping Module: Cyclic data transmission error */
            return QObject::tr("F37845 - HF Damping Module: Cyclic data transmission error");
            break;
        case 37850:
            /* F37850 - HF Damping Module: Internal software error */
            return QObject::tr("F37850 - HF Damping Module: Internal software error");
            break;
        case 37851:
            /* F37851 - HF Damping Module (CU): Sign-of-life missing */
            return QObject::tr("F37851 - HF Damping Module (CU): Sign-of-life missing");
            break;
        case 37860:
            /* F37860 - HF Damping Module (CU): Telegram error */
            return QObject::tr("F37860 - HF Damping Module (CU): Telegram error");
            break;
        case 37875:
            /* F37875 - HF Damping Module: power supply voltage failed */
            return QObject::tr("F37875 - HF Damping Module: power supply voltage failed");
            break;
        case 37885:
            /* F37885 - HF Damping Module (CU): Cyclic data transmission error */
            return QObject::tr("F37885 - HF Damping Module (CU): Cyclic data transmission error");
            break;
        case 37886:
            /* F37886 - HF Damping Module (CU): Error when sending DRIVE-CLiQ data */
            return QObject::tr("F37886 - HF Damping Module (CU): Error when sending DRIVE-CLiQ data");
            break;
        case 37887:
            /* F37887 - HF Damping Module (CU): Component faulted */
            return QObject::tr("F37887 - HF Damping Module (CU): Component faulted");
            break;
        case 37895:
            /* F37895 - HF Damping Module (CU): Alternating cyclic data transmission error */
            return QObject::tr("F37895 - HF Damping Module (CU): Alternating cyclic data transmission error");
            break;
        case 37896:
            /* F37896 - HF Damping Module (CU): Component properties inconsistent */
            return QObject::tr("F37896 - HF Damping Module (CU): Component properties inconsistent");
            break;
        case 37899:
            /* F37899 (N, A) - HF Damping Module: Unknown fault */
            return QObject::tr("F37899 (N, A) - HF Damping Module: Unknown fault");
            break;
        case 37903:
            /* F37903 - HF Damping Module: I2C bus error occurred */
            return QObject::tr("F37903 - HF Damping Module: I2C bus error occurred");
            break;
        case 37950:
            /* F37950 - HF Damping Module: Internal software error */
            return QObject::tr("F37950 - HF Damping Module: Internal software error");
            break;
        case 37999:
            /* A37999 (F, N) - HF Damping Module: Unknown alarm */
            return QObject::tr("A37999 (F, N) - HF Damping Module: Unknown alarm");
            break;
        case 40000:
            /* F40000 - Fault at DRIVE-CLiQ socket X100 */
            return QObject::tr("F40000 - Fault at DRIVE-CLiQ socket X100");
            break;
        case 40001:
            /* F40001 - Fault at DRIVE-CLiQ socket X101 */
            return QObject::tr("F40001 - Fault at DRIVE-CLiQ socket X101");
            break;
        case 40002:
            /* F40002 - Fault at DRIVE-CLiQ socket X102 */
            return QObject::tr("F40002 - Fault at DRIVE-CLiQ socket X102");
            break;
        case 40003:
            /* F40003 - Fault at DRIVE-CLiQ socket X103 */
            return QObject::tr("F40003 - Fault at DRIVE-CLiQ socket X103");
            break;
        case 40004:
            /* F40004 - Fault at DRIVE-CLiQ socket X104 */
            return QObject::tr("F40004 - Fault at DRIVE-CLiQ socket X104");
            break;
        case 40005:
            /* F40005 - Fault at DRIVE-CLiQ socket X105 */
            return QObject::tr("F40005 - Fault at DRIVE-CLiQ socket X105");
            break;
        case 40100:
            /* A40100 - Alarm at DRIVE-CLiQ socket X100 */
            return QObject::tr("A40100 - Alarm at DRIVE-CLiQ socket X100");
            break;
        case 40101:
            /* A40101 - Alarm at DRIVE-CLiQ socket X101 */
            return QObject::tr("A40101 - Alarm at DRIVE-CLiQ socket X101");
            break;
        case 40102:
            /* A40102 - Alarm at DRIVE-CLiQ socket X102 */
            return QObject::tr("A40102 - Alarm at DRIVE-CLiQ socket X102");
            break;
        case 40103:
            /* A40103 - Alarm at DRIVE-CLiQ socket X103 */
            return QObject::tr("A40103 - Alarm at DRIVE-CLiQ socket X103");
            break;
        case 40104:
            /* A40104 - Alarm at DRIVE-CLiQ socket X104 */
            return QObject::tr("A40104 - Alarm at DRIVE-CLiQ socket X104");
            break;
        case 40105:
            /* A40105 - Alarm at DRIVE-CLiQ socket X105 */
            return QObject::tr("A40105 - Alarm at DRIVE-CLiQ socket X105");
            break;
        case 40799:
            /* F40799 - CX32: Configured transfer end time exceeded */
            return QObject::tr("F40799 - CX32: Configured transfer end time exceeded");
            break;
        case 40801:
            /* F40801 - CX32 DRIVE-CLiQ: Sign-of-life missing */
            return QObject::tr("F40801 - CX32 DRIVE-CLiQ: Sign-of-life missing");
            break;
        case 40820:
            /* F40820 - CX32 DRIVE-CLiQ: Telegram error */
            return QObject::tr("F40820 - CX32 DRIVE-CLiQ: Telegram error");
            break;
        case 40825:
            /* F40825 - CX32 DRIVE-CLiQ: Supply voltage failed */
            return QObject::tr("F40825 - CX32 DRIVE-CLiQ: Supply voltage failed");
            break;
        case 40835:
            /* F40835 - CX32 DRIVE-CLiQ: Cyclic data transfer error */
            return QObject::tr("F40835 - CX32 DRIVE-CLiQ: Cyclic data transfer error");
            break;
        case 40836:
            /* F40836 - CX32 DRIVE-CLiQ: Send error for DRIVE-CLiQ data */
            return QObject::tr("F40836 - CX32 DRIVE-CLiQ: Send error for DRIVE-CLiQ data");
            break;
        case 40837:
            /* F40837 - CX32 DRIVE-CLiQ: Component fault */
            return QObject::tr("F40837 - CX32 DRIVE-CLiQ: Component fault");
            break;
        case 40845:
            /* F40845 - CX32 DRIVE-CLiQ: Cyclic data transfer error */
            return QObject::tr("F40845 - CX32 DRIVE-CLiQ: Cyclic data transfer error");
            break;
        case 40851:
            /* F40851 - CX32 DRIVE-CLiQ (CU): Sign-of-life missing */
            return QObject::tr("F40851 - CX32 DRIVE-CLiQ (CU): Sign-of-life missing");
            break;
        case 40860:
            /* F40860 - CX32 DRIVE-CLiQ (CU): Telegram error */
            return QObject::tr("F40860 - CX32 DRIVE-CLiQ (CU): Telegram error");
            break;
        case 40875:
            /* F40875 - CX32 DRIVE-CLiQ (CU): Supply voltage failed */
            return QObject::tr("F40875 - CX32 DRIVE-CLiQ (CU): Supply voltage failed");
            break;
        case 40885:
            /* F40885 - CX32 DRIVE-CLiQ (CU): Cyclic data transfer error */
            return QObject::tr("F40885 - CX32 DRIVE-CLiQ (CU): Cyclic data transfer error");
            break;
        case 40886:
            /* F40886 - CX32 DRIVE-CLiQ (CU): Error when sending DRIVE-CLiQ data */
            return QObject::tr("F40886 - CX32 DRIVE-CLiQ (CU): Error when sending DRIVE-CLiQ data");
            break;
        case 40887:
            /* F40887 - CX32 DRIVE-CLiQ (CU): Component fault */
            return QObject::tr("F40887 - CX32 DRIVE-CLiQ (CU): Component fault");
            break;
        case 40895:
            /* F40895 - CX32 DRIVE-CLiQ (CU): Cyclic data transfer error */
            return QObject::tr("F40895 - CX32 DRIVE-CLiQ (CU): Cyclic data transfer error");
            break;
        case 49150:
            /* F49150 - Cooling unit: Fault occurred */
            return QObject::tr("F49150 - Cooling unit: Fault occurred");
            break;
        case 49151:
            /* F49151 - Cooling unit: Conductivity has exceeded the fault threshold */
            return QObject::tr("F49151 - Cooling unit: Conductivity has exceeded the fault threshold");
            break;
        case 49152:
            /* F49152 - Cooling unit: ON command feedback signal missing */
            return QObject::tr("F49152 - Cooling unit: ON command feedback signal missing");
            break;
        case 49153:
            /* F49153 - Cooling unit: Liquid flow too low */
            return QObject::tr("F49153 - Cooling unit: Liquid flow too low");
            break;
        case 49154:
            /* F49154 (A) - Cooling unit: Liquid leak is present */
            return QObject::tr("F49154 (A) - Cooling unit: Liquid leak is present");
            break;
        case 49155:
            /* F49155 - Cooling unit: Power Stack Adapter, firmware version too old */
            return QObject::tr("F49155 - Cooling unit: Power Stack Adapter, firmware version too old");
            break;
        case 49156:
            /* F49156 - Cooling unit: Cooling liquid temperature has exceeded the fault threshold */
            return QObject::tr("F49156 - Cooling unit: Cooling liquid temperature has exceeded the fault threshold");
            break;
        case 49170:
            /* A49170 - Cooling unit: Alarm has occurred */
            return QObject::tr("A49170 - Cooling unit: Alarm has occurred");
            break;
        case 49171:
            /* A49171 (N) - Cooling unit: Conductivity has exceeded the alarm threshold */
            return QObject::tr("A49171 (N) - Cooling unit: Conductivity has exceeded the alarm threshold");
            break;
        case 49172:
            /* A49172 - Cooling unit: Conductivity actual value is not valid */
            return QObject::tr("A49172 - Cooling unit: Conductivity actual value is not valid");
            break;
        case 49173:
            /* A49173 - Cooling unit: Cooling liquid temperature has exceeded the alarm threshold */
            return QObject::tr("A49173 - Cooling unit: Cooling liquid temperature has exceeded the alarm threshold");
            break;
        case 49200:
            /* F49200 - Excitation group signal fault */
            return QObject::tr("F49200 - Excitation group signal fault");
            break;
        case 49201:
            /* A49201 (F) - Excitation group signal alarm */
            return QObject::tr("A49201 (F) - Excitation group signal alarm");
            break;
        case 49204:
            /* A49204 (N) - Excitation switch-off alarm */
            return QObject::tr("A49204 (N) - Excitation switch-off alarm");
            break;
        case 49205:
            /* A49205 - Excitation: incorrect parameterization of the brushless exciter */
            return QObject::tr("A49205 - Excitation: incorrect parameterization of the brushless exciter");
            break;
        case 49920:
            /* A49920 (F) - Protective breaker main circuit tripped */
            return QObject::tr("A49920 (F) - Protective breaker main circuit tripped");
            break;
        case 49921:
            /* A49921 (F) - Protective breaker redundant main circuit tripped */
            return QObject::tr("A49921 (F) - Protective breaker redundant main circuit tripped");
            break;
        case 49922:
            /* A49922 (F) - Protective breaker 24 V circuit has tripped */
            return QObject::tr("A49922 (F) - Protective breaker 24 V circuit has tripped");
            break;
        case 49923:
            /* A49923 (F) - Protective breaker terminal strip 24 V circuit has tripped */
            return QObject::tr("A49923 (F) - Protective breaker terminal strip 24 V circuit has tripped");
            break;
        case 49924:
            /* A49924 (F) - Protective breaker power unit supply circuit tripped */
            return QObject::tr("A49924 (F) - Protective breaker power unit supply circuit tripped");
            break;
        case 49926:
            /* A49926 (F) - Protective breaker synchronizing voltage tripped */
            return QObject::tr("A49926 (F) - Protective breaker synchronizing voltage tripped");
            break;
        case 49927:
            /* A49927 (F) - Protective breaker auxiliary fan circuit has tripped */
            return QObject::tr("A49927 (F) - Protective breaker auxiliary fan circuit has tripped");
            break;
        case 49933:
            /* A49933 (F) - Protective breaker excitation 230 V AC circuit tripped */
            return QObject::tr("A49933 (F) - Protective breaker excitation 230 V AC circuit tripped");
            break;
        case 49934:
            /* A49934 (F) - Protective breaker output cooling unit 230 V AC circuit tripped */
            return QObject::tr("A49934 (F) - Protective breaker output cooling unit 230 V AC circuit tripped");
            break;
        case 49935:
            /* A49935 (F) - Protective breaker power unit door solenoids 24 V circuit has tripped */
            return QObject::tr("A49935 (F) - Protective breaker power unit door solenoids 24 V circuit has tripped");
            break;
        case 49936:
            /* A49936 - Prot. breaker lighting supply/socket outlets 230V AC cct has tripped */
            return QObject::tr("A49936 - Prot. breaker lighting supply/socket outlets 230V AC cct has tripped");
            break;
        case 49937:
            /* A49937 (F) - UPS not ready */
            return QObject::tr("A49937 (F) - UPS not ready");
            break;
        case 49938:
            /* A49938 (F) - UPS battery operation */
            return QObject::tr("A49938 (F) - UPS battery operation");
            break;
        case 49939:
            /* A49939 (F) - UPS battery discharged */
            return QObject::tr("A49939 (F) - UPS battery discharged");
            break;
        case 49940:
            /* A49940 (F) - Protective breaker tripped PU supply 400 V circuit */
            return QObject::tr("A49940 (F) - Protective breaker tripped PU supply 400 V circuit");
            break;
        case 49941:
            /* A49941 (F) - Protective breaker anti-condensation heating tripped */
            return QObject::tr("A49941 (F) - Protective breaker anti-condensation heating tripped");
            break;
        case 49942:
            /* A49942 (F) - Protective breaker SITOP supply circuit tripped */
            return QObject::tr("A49942 (F) - Protective breaker SITOP supply circuit tripped");
            break;
        case 49998:
            /* A49998 - Recorder trigger event occurred */
            return QObject::tr("A49998 - Recorder trigger event occurred");
            break;
        case 50002:
            /* A50002 (F) - COMM BOARD: Alarm 2 */
            return QObject::tr("A50002 (F) - COMM BOARD: Alarm 2");
            break;
        case 50003:
            /* A50003 (F) - COMM BOARD: Alarm 3 */
            return QObject::tr("A50003 (F) - COMM BOARD: Alarm 3");
            break;
        case 50004:
            /* A50004 (F) - COMM BOARD: Alarm 4 */
            return QObject::tr("A50004 (F) - COMM BOARD: Alarm 4");
            break;
        case 50005:
            /* A50005 (F) - COMM BOARD: Alarm 5 */
            return QObject::tr("A50005 (F) - COMM BOARD: Alarm 5");
            break;
        case 50006:
            /* A50006 (F) - COMM BOARD: Alarm 6 */
            return QObject::tr("A50006 (F) - COMM BOARD: Alarm 6");
            break;
        case 50007:
            /* A50007 (F) - COMM BOARD: Alarm 7 */
            return QObject::tr("A50007 (F) - COMM BOARD: Alarm 7");
            break;
        case 50008:
            /* A50008 (F) - COMM BOARD: Alarm 8 */
            return QObject::tr("A50008 (F) - COMM BOARD: Alarm 8");
            break;
        case 50011:
            /* A50011 (F) - EtherNetIP/COMM BOARD: configuration error */
            return QObject::tr("A50011 (F) - EtherNetIP/COMM BOARD: configuration error");
            break;
        default:
            return QString("%1 - %2").arg(errorCode).arg(QObject::tr("Unknown error number"));
    }
}
}
}

#endif // SIEMENSERRORCODES_H

#ifndef BECKHOFFERRORCODES_H
#define BECKHOFFERRORCODES_H

#include <QString>

namespace Beckhoff
{
    
    namespace ADSState
    {
        enum ADSSTATE {
            ADSSTATE_INVALID = 0,
            ADSSTATE_IDLE = 1,
            ADSSTATE_RESET = 2,
            ADSSTATE_INIT = 3,
            ADSSTATE_START = 4,
            ADSSTATE_RUN = 5,
            ADSSTATE_STOP = 6,
            ADSSTATE_SAVECFG = 7,
            ADSSTATE_LOADCFG = 8,
            ADSSTATE_POWERFAILURE = 9,
            ADSSTATE_POWERGOOD = 10,
            ADSSTATE_ERROR = 11,
            ADSSTATE_SHUTDOWN = 12,
            ADSSTATE_SUSPEND = 13,
            ADSSTATE_RESUME = 14,
            ADSSTATE_CONFIG = 15,
            ADSSTATE_RECONFIG = 16,
            ADSSTATE_STOPPING = 17,
            ADSSTATE_INCOMPATIBLE = 18,
            ADSSTATE_EXCEPTION = 19,
            ADSSTATE_MAXSTATES
        };
    
        QString errorDescription(uint16_t errorCode)
        {
            switch (errorCode)
            {
                case 0:
                    return QObject::tr("ADS State Invalid");
                case 1:
                    return QObject::tr("ADS State Idle");
                case 2:
                    return QObject::tr("ADS State Reset");
                case 3:
                    return QObject::tr("ADS State Init");
                case 4:
                    return QObject::tr("ADS State Start");
                case 5:
                    return QObject::tr("ADS State Run");
                case 6:
                    return QObject::tr("ADS State Stop");
                case 7:
                    return QObject::tr("ADS State Save Config");
                case 8:
                    return QObject::tr("ADS State Load Config");
                case 9:
                    return QObject::tr("ADS State Power Failure");
                case 10:
                    return QObject::tr("ADS State Power Good");
                case 11:
                    return QObject::tr("ADS State Error");
                case 12:
                    return QObject::tr("ADS State Shutdown");
                case 13:
                    return QObject::tr("ADS State Suspend");
                case 14:
                    return QObject::tr("ADS State Resume");
                case 15:
                    return QObject::tr("ADS State Config");
                case 16:
                    return QObject::tr("ADS State Reconfig");
                case 17:
                    return QObject::tr("ADS State Stopping");
                case 18:
                    return QObject::tr("ADS State Incompatible");
                case 19:
                    return QObject::tr("ADS State Exception");
                default:
                    return QObject::tr("Unknwon ADS State: %1").arg(errorCode);
            }
        }
    }
    
    namespace NC
    {
        QString errorDescription(uint16_t errorCode)
        {
            switch (errorCode)
            {
                case 16384:
                    return QObject::tr("16384 - Internal error");
                case 16385:
                    return QObject::tr("16385 - Memory error");
                case 16386:
                    return QObject::tr("16386 - Nc retain data error (persistent data)");
                case 16400:
                    return QObject::tr("16400 - Channel identifier not allowed");
                case 16401:
                    return QObject::tr("16401 - Group identifier not allowed");
                case 16402:
                    return QObject::tr("16402 - Axis identifier not allowed");
                case 16403:
                    return QObject::tr("16403 - Encoder identifier not allowed");
                case 16404:
                    return QObject::tr("16404 - Controller identifier not allowed");
                case 16405:
                    return QObject::tr("16405 - Drive identifier not allowed");
                case 16406:
                    return QObject::tr("16406 - Table identifier not allowed");
                case 16416:
                    return QObject::tr("16416 - No process image");
                case 16417:
                    return QObject::tr("16417 - No process image");
                case 16418:
                    return QObject::tr("16418 - No process image");
                case 16419:
                    return QObject::tr("16419 - No process image");
                case 16420:
                    return QObject::tr("16420 - No process image");
                case 16421:
                    return QObject::tr("16421 - No process image");
                case 16432:
                    return QObject::tr("16432 - Coupling type not allowed");
                case 16433:
                    return QObject::tr("16433 - Axis type not allowed");
                case 16448:
                    return QObject::tr("16448 - Axis is incompatible");
                case 16464:
                    return QObject::tr("16464 - Channel not ready for operation");
                case 16465:
                    return QObject::tr("16465 - Group not ready for operation");
                case 16466:
                    return QObject::tr("16466 - Axis not ready for operation");
                case 16480:
                    return QObject::tr("16480 - Channel exists");
                case 16481:
                    return QObject::tr("16481 - Group exists");
                case 16482:
                    return QObject::tr("16482 - Axis exists");
                case 16483:
                    return QObject::tr("16483 - Table exists");
                case 16496:
                    return QObject::tr("16496 - Axis index not allowed");
                case 16497:
                    return QObject::tr("16497 - Axis index not allowed");
                case 16641:
                    return QObject::tr("16641 - Group index not allowed");
                case 16642:
                    return QObject::tr("16642 - Null pointer");
                case 16643:
                    return QObject::tr("16643 - No process image");
                case 16644:
                    return QObject::tr("16644 - M-function index not allowed");
                case 16645:
                    return QObject::tr("16645 - No memory");
                case 16646:
                    return QObject::tr("16646 - Not ready");
                case 16647:
                    return QObject::tr("16647 - Function/command not supported");
                case 16648:
                    return QObject::tr("16648 - Invalid parameter while starting");
                case 16649:
                    return QObject::tr("16649 - Channel function/command not executable");
                case 16650:
                    return QObject::tr("16650 - ItpGoAhead not executable");
                case 16656:
                    return QObject::tr("16656 - Error opening a file");
                case 16657:
                    return QObject::tr("16657 - Syntax error during loading");
                case 16658:
                    return QObject::tr("16658 - Syntax error during interpretation");
                case 16659:
                    return QObject::tr("16659 - Missing subroutine");
                case 16660:
                    return QObject::tr("16660 - Loading buffer of interpreter is too small");
                case 16661:
                    return QObject::tr("16661 - Symbolic");
                case 16662:
                    return QObject::tr("16662 - Symbolic");
                case 16663:
                    return QObject::tr("16663 - Subroutine incomplete");
                case 16664:
                    return QObject::tr("16664 - Error while loading the NC program");
                case 16665:
                    return QObject::tr("16665 - Error while loading the NC program");
                case 16672:
                    return QObject::tr("16672 - Divide by zero");
                case 16673:
                    return QObject::tr("16673 - Invalid circle parameterization");
                case 16674:
                    return QObject::tr("16674 - Invalid FPU-Operation");
                case 16688:
                    return QObject::tr("16688 - Stack overflow: subroutines");
                case 16689:
                    return QObject::tr("16689 - Stack underflow: subroutines");
                case 16690:
                    return QObject::tr("16690 - Stack overflow: arithmetic unit");
                case 16691:
                    return QObject::tr("16691 - Stack underflow: arithmetic unit");
                case 16704:
                    return QObject::tr("16704 - Register index not allowed");
                case 16705:
                    return QObject::tr("16705 - Unacceptable G-function index");
                case 16706:
                    return QObject::tr("16706 - Unacceptable M-function index");
                case 16707:
                    return QObject::tr("16707 - Unacceptable extended address");
                case 16708:
                    return QObject::tr("16708 - Unacceptable index to the internal H-function");
                case 16709:
                    return QObject::tr("16709 - Machine data value unacceptable");
                case 16720:
                    return QObject::tr("16720 - Cannot change tool params here");
                case 16721:
                    return QObject::tr("16721 - Cannot calculate tool compensation");
                case 16722:
                    return QObject::tr("16722 - Tool compensation");
                case 16723:
                    return QObject::tr("16723 - Tool compensation");
                case 16724:
                    return QObject::tr("16724 - Tool compensation");
                case 16725:
                    return QObject::tr("16725 - Tool compensation");
                case 16726:
                    return QObject::tr("16726 - Tool compensation");
                case 16727:
                    return QObject::tr("16727 - Tool compensation");
                case 16728:
                    return QObject::tr("16728 - Tool compensation");
                case 16729:
                    return QObject::tr("16729 - Tool compensation");
                case 16730:
                    return QObject::tr("16730 - Tool compensation");
                case 16731:
                    return QObject::tr("16731 - Tool compensation");
                case 16752:
                    return QObject::tr("16752 - Error while loading: Invalid parameter");
                case 16753:
                    return QObject::tr("16753 - Invalid contour start position");
                case 16754:
                    return QObject::tr("16754 - Retrace: Invalid internal entry index");
                case 16896:
                    return QObject::tr("16896 - Group ID not allowed");
                case 16897:
                    return QObject::tr("16897 - Group type not allowed");
                case 16898:
                    return QObject::tr("16898 - Master axis index not allowed");
                case 16899:
                    return QObject::tr("16899 - Slave axis index not allowed");
                case 16900:
                    return QObject::tr("16900 - INTERNAL ERROR");
                case 16901:
                    return QObject::tr("16901 - Invalid cycle time for statement execution task (SAF)");
                case 16903:
                    return QObject::tr("16903 - Invalid cycle time for the statement preparation task (SVB)");
                case 16904:
                    return QObject::tr("16904 - Single step mode not allowed");
                case 16905:
                    return QObject::tr("16905 - Group deactivation not allowed");
                case 16906:
                    return QObject::tr("16906 - Statement execution state (SAF state) not allowed");
                case 16907:
                    return QObject::tr("16907 - Channel address");
                case 16908:
                    return QObject::tr("16908 - Axis address (master axis)");
                case 16909:
                    return QObject::tr("16909 - Master axis address");
                case 16910:
                    return QObject::tr("16910 - Slave axis address");
                case 16911:
                    return QObject::tr("16911 - Slave set value generator address");
                case 16912:
                    return QObject::tr("16912 - Encoder address");
                case 16913:
                    return QObject::tr("16913 - Controller address");
                case 16914:
                    return QObject::tr("16914 - Drive address");
                case 16915:
                    return QObject::tr("16915 - GROUPERR_ADDR_MASTERGENERATOR");
                case 16916:
                    return QObject::tr("16916 - Axis interface NC to PLC address");
                case 16917:
                    return QObject::tr("16917 - Slave axis address");
                case 16918:
                    return QObject::tr("16918 - Table address unknown");
                case 16919:
                    return QObject::tr("16919 - NcControl address");
                case 16920:
                    return QObject::tr("16920 - Axis is blocked for commands while persistent NC data are queued");
                case 16921:
                    return QObject::tr("16921 - The scaling mode MASTER-AUTOOFFSET is invalid because no reference table was found");
                case 16922:
                    return QObject::tr("16922 - The master axis start position does not permit synchronization");
                case 16923:
                    return QObject::tr("16923 - Slave coupling factor (gearing factor) of 0.0 is not allowed");
                case 16924:
                    return QObject::tr("16924 - Insertion of master axis into group not allowed");
                case 16925:
                    return QObject::tr("16925 - Deletion of master axis from group not allowed");
                case 16926:
                    return QObject::tr("16926 - Function/feature is not supported from the setpoint generator");
                case 16927:
                    return QObject::tr("16927 - Group initialization");
                case 16928:
                    return QObject::tr("16928 - Group not ready / group not ready for new task");
                case 16929:
                    return QObject::tr("16929 - Requested set velocity is not allowed");
                case 16930:
                    return QObject::tr("16930 - Requested target position is not allowed (master axis)");
                case 16931:
                    return QObject::tr("16931 - No enable for controller and/or feed");
                case 16932:
                    return QObject::tr("16932 - Movement smaller than one encoder increment");
                case 16933:
                    return QObject::tr("16933 - Drive not ready during axis start");
                case 16934:
                    return QObject::tr("16934 - Invalid parameters of the emergency stop");
                case 16935:
                    return QObject::tr("16935 - The setpoint generator is inactive such that no instructions are accepted");
                case 16936:
                    return QObject::tr("16936 - Requested traverse distance is not allowed");
                case 16937:
                    return QObject::tr("16937 - Requested target position is not allowed (slave axis)");
                case 16938:
                    return QObject::tr("16938 - No enable for controller and/or feed (slave axis)");
                case 16939:
                    return QObject::tr("16939 - The activation position (position threshold) is out of range of the actual positioning");
                case 16940:
                    return QObject::tr("16940 - The start or activation data of the external setpoint generation are not valid");
                case 16941:
                    return QObject::tr("16941 - Velocity is not constant");
                case 16942:
                    return QObject::tr("16942 - Jerk less than or equal to 0.0 is not allowed");
                case 16943:
                    return QObject::tr("16943 - Acceleration less than or equal to 0.0 is not allowed");
                case 16944:
                    return QObject::tr("16944 - Absolute deceleration value less than or equal to 0.0 is not allowed");
                case 16945:
                    return QObject::tr("16945 - Set velocity less than or equal to 0.0 is not allowed");
                case 16946:
                    return QObject::tr("16946 - Loss of precision when trying a positioning");
                case 16947:
                    return QObject::tr("16947 - Cycle time less than or equal to 0.0 is not allowed");
                case 16948:
                    return QObject::tr("16948 - PTP data type <intasdouble> range exceeded");
                case 16949:
                    return QObject::tr("16949 - PTP LHL velocity profile cannot be generated");
                case 16950:
                    return QObject::tr("16950 - PTP HML velocity profile cannot be generated");
                case 16951:
                    return QObject::tr("16951 - Start data address is invalid");
                case 16952:
                    return QObject::tr("16952 - Velocity override (start override) is not allowed");
                case 16953:
                    return QObject::tr("16953 - Start type not allowed");
                case 16954:
                    return QObject::tr("16954 - Velocity overflow (overshoot in the velocity)");
                case 16955:
                    return QObject::tr("16955 - Start parameter for the axis structure is invalid");
                case 16956:
                    return QObject::tr("16956 - Override generator initialization parameter invalid");
                case 16957:
                    return QObject::tr("16957 - Slave axis has not set value generator");
                case 16958:
                    return QObject::tr("16958 - Table is empty");
                case 16959:
                    return QObject::tr("16959 - Table is full");
                case 16960:
                    return QObject::tr("16960 - No memory available");
                case 16961:
                    return QObject::tr("16961 - Table already contains an entry");
                case 16962:
                    return QObject::tr("16962 - Stop is already active");
                case 16963:
                    return QObject::tr("16963 - Compensation has not been carried out over the full compensation section");
                case 16964:
                    return QObject::tr("16964 - Internal parameters for the compensation are invalid");
                case 16965:
                    return QObject::tr("16965 - Compensation active");
                case 16966:
                    return QObject::tr("16966 - Compensation not active");
                case 16967:
                    return QObject::tr("16967 - Compensation type invalid");
                case 16968:
                    return QObject::tr("16968 - Axis address for compensation invalid");
                case 16969:
                    return QObject::tr("16969 - Invalid slave address");
                case 16970:
                    return QObject::tr("16970 - Coupling velocity invalid");
                case 16971:
                    return QObject::tr("16971 - Coupling velocities not constant");
                case 16972:
                    return QObject::tr("16972 - Cycle time less than or equal to 0.0 is not allowed");
                case 16973:
                    return QObject::tr("16973 - Decoupling task not allowed");
                case 16974:
                    return QObject::tr("16974 - Function not allowed");
                case 16975:
                    return QObject::tr("16975 - No valid table weighting has been set");
                case 16976:
                    return QObject::tr("16976 - Axis type, actual position type or end position type is not allowed");
                case 16977:
                    return QObject::tr("16977 - Function not presently supported");
                case 16978:
                    return QObject::tr("16978 - State of state machine invalid");
                case 16979:
                    return QObject::tr("16979 - Reference cam became free too soon");
                case 16980:
                    return QObject::tr("16980 - Clearance monitoring between activation of the hardware latch and appearance of the sync pulse");
                case 16981:
                    return QObject::tr("16981 - No memory available");
                case 16982:
                    return QObject::tr("16982 - The table slave axis has no active table");
                case 16983:
                    return QObject::tr("16983 - Function not allowed");
                case 16984:
                    return QObject::tr("16984 - Stopping compensation not allowed");
                case 16985:
                    return QObject::tr("16985 - Slave table is being used");
                case 16986:
                    return QObject::tr("16986 - Master or slave axis is processing a job (e.g. positioning command) while coupling is requested");
                case 16987:
                    return QObject::tr("16987 - Slave (start) parameter is incorrect");
                case 16988:
                    return QObject::tr("16988 - Slave type is incorrect");
                case 16989:
                    return QObject::tr("16989 - Axis stop is already active");
                case 16990:
                    return QObject::tr("16990 - Maximum number of tables per slavegenerator reached");
                case 16991:
                    return QObject::tr("16991 - The scaling mode is invalid");
                case 16992:
                    return QObject::tr("16992 - Controller enable");
                case 16993:
                    return QObject::tr("16993 - Table not found");
                case 16994:
                    return QObject::tr("16994 - Incorrect table type");
                case 16995:
                    return QObject::tr("16995 - Single step mode");
                case 16996:
                    return QObject::tr("16996 - Group task unknown (asynchronous table entry)");
                case 16997:
                    return QObject::tr("16997 - Group function unknown (synchronous function)");
                case 16998:
                    return QObject::tr("16998 - Group task for slave not allowed");
                case 16999:
                    return QObject::tr("16999 - Group function for slave not allowed");
                case 17000:
                    return QObject::tr("17000 - GROUPERR_GROUPFUNC_NOMOTION");
                case 17001:
                    return QObject::tr("17001 - Startposition=Setpoint Position");
                case 17002:
                    return QObject::tr("17002 - Parameters of the delay-generator are invalid");
                case 17003:
                    return QObject::tr("17003 - External parameters of the superimposed instruction are invalid");
                case 17004:
                    return QObject::tr("17004 - Invalid override type");
                case 17005:
                    return QObject::tr("17005 - Activation position under/overrun");
                case 17006:
                    return QObject::tr("17006 - Activation impossible: Master is standing");
                case 17007:
                    return QObject::tr("17007 - Activation mode not possible");
                case 17008:
                    return QObject::tr("17008 - Start parameter for the compensation is invalid");
                case 17009:
                    return QObject::tr("17009 - Start parameter for the compensation is invalid");
                case 17010:
                    return QObject::tr("17010 - Start parameter for the compensation is invalid");
                case 17011:
                    return QObject::tr("17011 - Target position under/overrun");
                case 17012:
                    return QObject::tr("17012 - Target position will be under/overrun");
                case 17014:
                    return QObject::tr("17014 - GROUPERR_GUIDERSTARTDATA");
                case 17015:
                    return QObject::tr("17015 - Dynamic parameters not permitted");
                case 17017:
                    return QObject::tr("17017 - GROUPERR_GUIDEROVERRUN");
                case 17018:
                    return QObject::tr("17018 - GROUPERR_GUIDERLOOKAHEAD");
                case 17019:
                    return QObject::tr("17019 - GROUPERR_GUIDERLOOKAHEADEND");
                case 17020:
                    return QObject::tr("17020 - GROUPERR_GUIDERLOOKAHEADREQU");
                case 17021:
                    return QObject::tr("17021 - GROUPERR_GUIDERMODE");
                case 17022:
                    return QObject::tr("17022 - A requested motion command could not be realized (BISECTION)");
                case 17023:
                    return QObject::tr("17023 - The new target position either has been overrun or will be overrun");
                case 17024:
                    return QObject::tr("17024 - Group not ready / group not ready for new task");
                case 17025:
                    return QObject::tr("17025 - The parameters of the oriented stop (O-Stop) are not admitted");
                case 17026:
                    return QObject::tr("17026 - The modulo target position of the modulo-start is invalid");
                case 17027:
                    return QObject::tr("17027 - The online change activation mode is invalid");
                case 17028:
                    return QObject::tr("17028 - The parameterized jerk rate is not permitted");
                case 17029:
                    return QObject::tr("17029 - The parameterized acceleration or deceleration is not permitted");
                case 17030:
                    return QObject::tr("17030 - The parameterized velocity is not permitted");
                case 17031:
                    return QObject::tr("17031 - A activation cannot be executed due to a pending activation");
                case 17032:
                    return QObject::tr("17032 - Illegal combination of different cycle times within an axis group");
                case 17033:
                    return QObject::tr("17033 - Illegal motion reversal");
                case 17034:
                    return QObject::tr("17034 - Illegal moment for an axis command because there is an old axis command with activation position still active");
                case 17035:
                    return QObject::tr("17035 - Error in the stop-calculation routine");
                case 17036:
                    return QObject::tr("17036 - A command with activation position cannot fully be performed because the remaining path is too short");
                case 17037:
                    return QObject::tr("17037 - Illegal decouple type when decoupling a slave axis");
                case 17038:
                    return QObject::tr("17038 - Illegal target velocity when decoupling a slave axis");
                case 17039:
                    return QObject::tr("17039 - The command new dynamic parameter cannot be performed since this would require a new target velocity");
                case 17040:
                    return QObject::tr("17040 - A command with activation position cannot be performed because the axis is already in the brake phase");
                case 17041:
                    return QObject::tr("17041 - Decouple routine of slave axis doesn't return a valid solution");
                case 17042:
                    return QObject::tr("17042 - Command not be executed because the command buffer is full filled");
                case 17043:
                    return QObject::tr("17043 - Command is rejected due to an internal error in the Look Ahead");
                case 17044:
                    return QObject::tr("17044 - Command is rejected because the segment target velocity is not realized");
                case 17045:
                    return QObject::tr("17045 - Successive commands have the same final position");
                case 17046:
                    return QObject::tr("17046 - Logical positioning direction is inconsistent with the direction of the buffer command");
                case 17047:
                    return QObject::tr("17047 - Command is rejected because the remaining positioning length is to small");
                case 17051:
                    return QObject::tr("17051 - Collect error for invalid start parameters");
                case 17052:
                    return QObject::tr("17052 - Reference cam is not found");
                case 17053:
                    return QObject::tr("17053 - Reference cam became not free");
                case 17054:
                    return QObject::tr("17054 - IO sync pulse was not found (only when using hardware latch)");
                case 17056:
                    return QObject::tr("17056 - Group/axis consequential error");
                case 17057:
                    return QObject::tr("17057 - Velocity reduction factor for C0/C1 transition is not allowed");
                case 17058:
                    return QObject::tr("17058 - Critical angle at segment transition not allowed");
                case 17059:
                    return QObject::tr("17059 - Radius of the tolerance sphere");
                case 17061:
                    return QObject::tr("17061 - Start type");
                case 17063:
                    return QObject::tr("17063 - Blending");
                case 17065:
                    return QObject::tr("17065 - Curve velocity reduction method not allowed");
                case 17066:
                    return QObject::tr("17066 - Minimum velocity not allowed");
                case 17067:
                    return QObject::tr("17067 - Power function input not allowed");
                case 17068:
                    return QObject::tr("17068 - Dynamic change parameter not allowed");
                case 17069:
                    return QObject::tr("17069 - Memory allocation error");
                case 17070:
                    return QObject::tr("17070 - The calculated end position differs from the end position in the nc instruction (internal error)");
                case 17071:
                    return QObject::tr("17071 - Calculate remaining chord length");
                case 17072:
                    return QObject::tr("17072 - Set value generator SVB active");
                case 17073:
                    return QObject::tr("17073 - SVB parameter not allowed");
                case 17074:
                    return QObject::tr("17074 - Velocity reduction factor not allowed");
                case 17075:
                    return QObject::tr("17075 - Helix is a circle");
                case 17076:
                    return QObject::tr("17076 - Helix is a straight line");
                case 17077:
                    return QObject::tr("17077 - Guider parameter not allowed");
                case 17078:
                    return QObject::tr("17078 - Invalid segment address");
                case 17079:
                    return QObject::tr("17079 - Unparameterized generator");
                case 17080:
                    return QObject::tr("17080 - Unparameterized table");
                case 17082:
                    return QObject::tr("17082 - The calculation of the arc length of the smoothed path failed (internal error)");
                case 17083:
                    return QObject::tr("17083 - The radius of the tolerance ball is too small (smaller than 0.1 mm)");
                case 17085:
                    return QObject::tr("17085 - NC-Block violates software limit switches of the group");
                case 17086:
                    return QObject::tr("17086 - GROUPERR_DXD_SOFTENDCHECK");
                case 17087:
                    return QObject::tr("17087 - GROUPERR_DXD_RTTG_VELOREFERENCE");
                case 17088:
                    return QObject::tr("17088 - Interpolating group contains axes of an incorrect axis type");
                case 17089:
                    return QObject::tr("17089 - Scalar product cannot be calculated");
                case 17090:
                    return QObject::tr("17090 - Inverse cosine cannot be calculated");
                case 17091:
                    return QObject::tr("17091 - Invalid table entry type");
                case 17092:
                    return QObject::tr("17092 - Invalid DIN66025 information type");
                case 17093:
                    return QObject::tr("17093 - Invalid dimension");
                case 17094:
                    return QObject::tr("17094 - Geometrical object is not a straight line");
                case 17095:
                    return QObject::tr("17095 - Geometrical object is not a circle");
                case 17096:
                    return QObject::tr("17096 - Geometrical object is not a helix");
                case 17097:
                    return QObject::tr("17097 - Set velocity less than or equal to 0.0 is invalid");
                case 17098:
                    return QObject::tr("17098 - Address for look-ahead invalid");
                case 17099:
                    return QObject::tr("17099 - Set value generator SAF active");
                case 17100:
                    return QObject::tr("17100 - CNC set value generation not active");
                case 17101:
                    return QObject::tr("17101 - CNC set value generation in the stop phase");
                case 17102:
                    return QObject::tr("17102 - Override not allowed");
                case 17103:
                    return QObject::tr("17103 - Invalid table address");
                case 17104:
                    return QObject::tr("17104 - Invalid table entry type");
                case 17105:
                    return QObject::tr("17105 - Memory allocation failed");
                case 17106:
                    return QObject::tr("17106 - Memory allocation failed");
                case 17107:
                    return QObject::tr("17107 - Invalid parameter");
                case 17108:
                    return QObject::tr("17108 - Delete Distance To Go failed");
                case 17109:
                    return QObject::tr("17109 - The setpoint generator of the flying saw generates incompatible values (internal error)");
                case 17110:
                    return QObject::tr("17110 - Axis will be stopped since otherwise it will overrun its target position (old PTP setpoint generator)");
                case 17111:
                    return QObject::tr("17111 - Internal error in the transformation from slave to master");
                case 17112:
                    return QObject::tr("17112 - Wrong direction in the transformation of slave to master");
                case 17114:
                    return QObject::tr("17114 - Parameter of Motion Function (MF) table incorrect");
                case 17115:
                    return QObject::tr("17115 - Parameter of Motion Function (MF) table incorrect");
                case 17116:
                    return QObject::tr("17116 - Internal error by using Motion Function (MF)");
                case 17117:
                    return QObject::tr("17117 - Axis coupling with synchronization generator declined because of incorrect axis dynamic values");
                case 17118:
                    return QObject::tr("17118 - Coupling conditions of synchronization generator incorrect");
                case 17119:
                    return QObject::tr("17119 - Moving profile of synchronization generator declines dynamic limit of slave axis or required characteristic of profile");
                case 17120:
                    return QObject::tr("17120 - Invalid parameter");
                case 17121:
                    return QObject::tr("17121 - Invalid parameter");
                case 17122:
                    return QObject::tr("17122 - External generator is active");
                case 17123:
                    return QObject::tr("17123 - External generator is not active");
                case 17124:
                    return QObject::tr("17124 - NC-Block with auxiliary axis violates software limit switches of the group");
                case 17125:
                    return QObject::tr("17125 - NC-Block type Bezier spline curve contains a cusp (singularity)");
                case 17127:
                    return QObject::tr("17127 - Value for dead time compensation not allowed");
                case 17128:
                    return QObject::tr("17128 - GROUPERR_RANGE_NOMOTIONWINDOW");
                case 17129:
                    return QObject::tr("17129 - GROUPERR_RANGE_NOMOTIONFILTERTIME");
                case 17130:
                    return QObject::tr("17130 - GROUPERR_RANGE_TIMEUNITFIFO");
                case 17131:
                    return QObject::tr("17131 - GROUPERR_RANGE_OVERRIDETYPE");
                case 17132:
                    return QObject::tr("17132 - GROUPERR_RANGE_OVERRIDECHANGETIME");
                case 17133:
                    return QObject::tr("17133 - GROUPERR_FIFO_INVALIDDIMENSION");
                case 17134:
                    return QObject::tr("17134 - GROUPERR_ADDR_FIFOTABLE");
                case 17135:
                    return QObject::tr("17135 - Axis is locked for motion commands because a stop command is still active");
                case 17136:
                    return QObject::tr("17136 - Invalid number of auxiliary axes");
                case 17137:
                    return QObject::tr("17137 - Invalid reduction parameter for auxiliary axes");
                case 17138:
                    return QObject::tr("17138 - Invalid dynamic parameter for auxiliary axes");
                case 17139:
                    return QObject::tr("17139 - Invalid coupling parameter for auxiliary axes");
                case 17140:
                    return QObject::tr("17140 - Invalid auxiliary axis entry");
                case 17142:
                    return QObject::tr("17142 - Invalid parameter");
                case 17144:
                    return QObject::tr("17144 - Block search - segment not found");
                case 17145:
                    return QObject::tr("17145 - Blocksearch – invalid remaining segment length");
                case 17147:
                    return QObject::tr("17147 - INTERNAL ERROR");
                case 17151:
                    return QObject::tr("17151 - GROUPERR_WATCHDOG");
                case 17152:
                    return QObject::tr("17152 - Axis ID not allowed");
                case 17153:
                    return QObject::tr("17153 - Axis type not allowed");
                case 17158:
                    return QObject::tr("17158 - Slow manual velocity not allowed");
                case 17159:
                    return QObject::tr("17159 - Fast manual velocity not allowed");
                case 17160:
                    return QObject::tr("17160 - High speed not allowed");
                case 17161:
                    return QObject::tr("17161 - Acceleration not allowed");
                case 17162:
                    return QObject::tr("17162 - Deceleration not allowed");
                case 17163:
                    return QObject::tr("17163 - Jerk not allowed");
                case 17164:
                    return QObject::tr("17164 - Delay time between position and velocity is not allowed");
                case 17165:
                    return QObject::tr("17165 - Override-Type not allowed");
                case 17166:
                    return QObject::tr("17166 - NCI: Velo-Jump-Factor not allowed");
                case 17167:
                    return QObject::tr("17167 - NCI: Radius of tolerance sphere for the auxiliary axes is invalid");
                case 17168:
                    return QObject::tr("17168 - NCI: Value for maximum deviation for the auxiliary axes is invalid");
                case 17170:
                    return QObject::tr("17170 - Referencing velocity in direction of cam not allowed");
                case 17171:
                    return QObject::tr("17171 - Referencing velocity in sync direction not allowed");
                case 17172:
                    return QObject::tr("17172 - Pulse width in positive direction not allowed");
                case 17173:
                    return QObject::tr("17173 - Pulse width in negative direction not allowed");
                case 17174:
                    return QObject::tr("17174 - Pulse time in positive direction not allowed");
                case 17175:
                    return QObject::tr("17175 - Pulse time in negative direction not allowed");
                case 17176:
                    return QObject::tr("17176 - Creep distance in positive direction not allowed");
                case 17177:
                    return QObject::tr("17177 - Creep distance in negative direction not allowed");
                case 17178:
                    return QObject::tr("17178 - Braking distance in positive direction not allowed");
                case 17179:
                    return QObject::tr("17179 - Braking distance in negative direction not allowed");
                case 17180:
                    return QObject::tr("17180 - Braking time in positive direction not allowed");
                case 17181:
                    return QObject::tr("17181 - Braking time in negative direction not allowed");
                case 17182:
                    return QObject::tr("17182 - Switching time from high to low speed not allowed");
                case 17183:
                    return QObject::tr("17183 - Creep distance for stop not allowed");
                case 17184:
                    return QObject::tr("17184 - Motion monitoring not allowed");
                case 17185:
                    return QObject::tr("17185 - Position window monitoring not allowed");
                case 17186:
                    return QObject::tr("17186 - Target window monitoring not allowed");
                case 17187:
                    return QObject::tr("17187 - Loop not allowed");
                case 17188:
                    return QObject::tr("17188 - Motion monitoring time not allowed");
                case 17189:
                    return QObject::tr("17189 - Target window range not allowed");
                case 17190:
                    return QObject::tr("17190 - Position window range not allowed");
                case 17191:
                    return QObject::tr("17191 - Position window monitoring time not allowed");
                case 17192:
                    return QObject::tr("17192 - Loop movement not allowed");
                case 17193:
                    return QObject::tr("17193 - Axis cycle time not allowed");
                case 17194:
                    return QObject::tr("17194 - Stepper motor operating mode not allowed");
                case 17195:
                    return QObject::tr("17195 - Displacement per stepper motor step not allowed");
                case 17196:
                    return QObject::tr("17196 - Minimum speed for stepper motor set value profile not allowed");
                case 17197:
                    return QObject::tr("17197 - Stepper motor stages for one speed stage not allowed");
                case 17198:
                    return QObject::tr("17198 - DWORD for the interpretation of the axis units not allowed");
                case 17199:
                    return QObject::tr("17199 - Maximum velocity not allowed");
                case 17200:
                    return QObject::tr("17200 - Motion monitoring window not allowed");
                case 17201:
                    return QObject::tr("17201 - PEH time monitoring not allowed");
                case 17202:
                    return QObject::tr("17202 - PEH monitoring time not allowed");
                case 17203:
                    return QObject::tr("17203 - AXISERR_RANGE_DELAYBREAKRELEASE");
                case 17204:
                    return QObject::tr("17204 - AXISERR_RANGE_DATAPERSISTENCE");
                case 17210:
                    return QObject::tr("17210 - AXISERR_RANGE_POSDIFF_FADING_ACCELERATION");
                case 17211:
                    return QObject::tr("17211 - Fast Axis Stop Signal Type not allowed");
                case 17216:
                    return QObject::tr("17216 - Axis initialization");
                case 17217:
                    return QObject::tr("17217 - Group address");
                case 17218:
                    return QObject::tr("17218 - Encoder address");
                case 17219:
                    return QObject::tr("17219 - Controller address");
                case 17220:
                    return QObject::tr("17220 - Drive address");
                case 17221:
                    return QObject::tr("17221 - Axis interface PLC to NC address");
                case 17222:
                    return QObject::tr("17222 - Axis interface NC to PLC address");
                case 17223:
                    return QObject::tr("17223 - Size of axis interface NC to PLC is not allowed");
                case 17224:
                    return QObject::tr("17224 - Size of axis interface PLC to NC is not allowed");
                case 17238:
                    return QObject::tr("17238 - Controller enable");
                case 17239:
                    return QObject::tr("17239 - Feed enable minus");
                case 17240:
                    return QObject::tr("17240 - Feed enable plus");
                case 17241:
                    return QObject::tr("17241 - Set velocity not allowed");
                case 17242:
                    return QObject::tr("17242 - Movement smaller than one encoder increment");
                case 17243:
                    return QObject::tr("17243 - Set acceleration monitoring");
                case 17244:
                    return QObject::tr("17244 - PEH time monitoring");
                case 17245:
                    return QObject::tr("17245 - Encoder existence monitoring / movement monitoring");
                case 17246:
                    return QObject::tr("17246 - Looping distance less than breaking distance");
                case 17249:
                    return QObject::tr("17249 - Time range exceeded (future)");
                case 17250:
                    return QObject::tr("17250 - Time range exceeded (past)");
                case 17251:
                    return QObject::tr("17251 - Position cannot be determined");
                case 17252:
                    return QObject::tr("17252 - Position indeterminable (conflicting direction of travel)");
                case 17312:
                    return QObject::tr("17312 - Axis consequential error");
                case 17408:
                    return QObject::tr("17408 - Encoder ID not allowed");
                case 17409:
                    return QObject::tr("17409 - Encoder type not allowed");
                case 17410:
                    return QObject::tr("17410 - Encoder mode");
                case 17411:
                    return QObject::tr("17411 - Encoder counting direction inverted?");
                case 17412:
                    return QObject::tr("17412 - Referencing status");
                case 17413:
                    return QObject::tr("17413 - Encoder increments for each physical encoder rotation");
                case 17414:
                    return QObject::tr("17414 - Scaling factor");
                case 17415:
                    return QObject::tr("17415 - Position offset (zero point offset)");
                case 17416:
                    return QObject::tr("17416 - Modulo factor");
                case 17417:
                    return QObject::tr("17417 - Position filter time");
                case 17418:
                    return QObject::tr("17418 - Velocity filter time");
                case 17419:
                    return QObject::tr("17419 - Acceleration filter time");
                case 17420:
                    return QObject::tr("17420 - Cycle time not allowed");
                case 17422:
                    return QObject::tr("17422 - Actual position correction / measurement system error correction");
                case 17423:
                    return QObject::tr("17423 - Filter time actual position correction");
                case 17424:
                    return QObject::tr("17424 - Search direction for referencing cam inverted");
                case 17425:
                    return QObject::tr("17425 - Search direction for sync pulse (zero pulse) inverted");
                case 17426:
                    return QObject::tr("17426 - Reference position");
                case 17427:
                    return QObject::tr("17427 - Clearance monitoring between activation of the hardware latch and appearance of the sync pulse");
                case 17428:
                    return QObject::tr("17428 - Minimum clearance between activation of the hardware latch and appearance of the sync pulse");
                case 17429:
                    return QObject::tr("17429 - External sync pulse");
                case 17430:
                    return QObject::tr("17430 - Scaling of the noise rate is not allowed");
                case 17431:
                    return QObject::tr("17431 - Tolerance window for modulo-start");
                case 17432:
                    return QObject::tr("17432 - Encoder reference mode");
                case 17433:
                    return QObject::tr("17433 - Encoder evaluation direction");
                case 17434:
                    return QObject::tr("17434 - Encoder reference system");
                case 17435:
                    return QObject::tr("17435 - Encoder position initialization mode");
                case 17436:
                    return QObject::tr("17436 - Encoder sign interpretation (UNSIGNED- / SIGNED- data type)");
                case 17440:
                    return QObject::tr("17440 - Software end location monitoring minimum not allowed");
                case 17441:
                    return QObject::tr("17441 - Software end location monitoring maximum not allowed");
                case 17442:
                    return QObject::tr("17442 - Actual value setting is outside the value range");
                case 17443:
                    return QObject::tr("17443 - Software end location minimum not allowed");
                case 17444:
                    return QObject::tr("17444 - Software end location maximum not allowed");
                case 17445:
                    return QObject::tr("17445 - Filter mask for the raw data of the encoder is invalid");
                case 17446:
                    return QObject::tr("17446 - Reference mask for the raw data of the encoder is invalid");
                case 17456:
                    return QObject::tr("17456 - Hardware latch activation (encoder)");
                case 17457:
                    return QObject::tr("17457 - External hardware latch activation (encoder)");
                case 17458:
                    return QObject::tr("17458 - External hardware latch activation (encoder)");
                case 17459:
                    return QObject::tr("17459 - External hardware latch activation (encoder)");
                case 17460:
                    return QObject::tr("17460 - Encoder function not supported");
                case 17461:
                    return QObject::tr("17461 - Encoder function is already active");
                case 17472:
                    return QObject::tr("17472 - Encoder initialization");
                case 17473:
                    return QObject::tr("17473 - Axis address");
                case 17474:
                    return QObject::tr("17474 - I/O input structure address");
                case 17475:
                    return QObject::tr("17475 - I/O output structure address");
                case 17488:
                    return QObject::tr("17488 - Encoder counter underflow monitoring");
                case 17489:
                    return QObject::tr("17489 - Encoder counter overflow monitoring");
                case 17504:
                    return QObject::tr("17504 - Software end location minimum (axis start)");
                case 17505:
                    return QObject::tr("17505 - Software end location maximum (axis start)");
                case 17506:
                    return QObject::tr("17506 - Software end location minimum (positioning process)");
                case 17507:
                    return QObject::tr("17507 - Software end location maximum (positioning process)");
                case 17508:
                    return QObject::tr("17508 - Encoder hardware error");
                case 17509:
                    return QObject::tr("17509 - Position initialization error at system start");
                case 17520:
                    return QObject::tr("17520 - SSI transformation fault or not finished");
                case 17570:
                    return QObject::tr("17570 - ENCERR_ADDR_CONTROLLER");
                case 17571:
                    return QObject::tr("17571 - ENCERR_INVALID_CONTROLLERTYPE");
                case 17664:
                    return QObject::tr("17664 - Controller ID not allowed");
                case 17665:
                    return QObject::tr("17665 - Controller type not allowed");
                case 17666:
                    return QObject::tr("17666 - Controller operating mode not allowed");
                case 17667:
                    return QObject::tr("17667 - Weighting of the velocity pre-control not allowed");
                case 17668:
                    return QObject::tr("17668 - Following error monitoring (position) not allowed");
                case 17669:
                    return QObject::tr("17669 - Following error (velocity) not allowed");
                case 17670:
                    return QObject::tr("17670 - Following error window (position) not allowed");
                case 17671:
                    return QObject::tr("17671 - Following error filter time (position) not allowed");
                case 17672:
                    return QObject::tr("17672 - Following error window (velocity) not allowed");
                case 17673:
                    return QObject::tr("17673 - Following error filter time (velocity) not allowed");
                case 17680:
                    return QObject::tr("17680 - Proportional gain Kv or Kp (controller) not allowed");
                case 17681:
                    return QObject::tr("17681 - Integral-action time Tn (controller) not allowed");
                case 17682:
                    return QObject::tr("17682 - Derivative action time Tv (controller) not allowed");
                case 17683:
                    return QObject::tr("17683 - Damping time Td (controller) not allowed");
                case 17684:
                    return QObject::tr("17684 - Activation of the automatic offset compensation not allowed");
                case 17685:
                    return QObject::tr("17685 - Additional proportional gain Kv or Kp (controller) not allowed");
                case 17686:
                    return QObject::tr("17686 - Reference velocity for additional proportional gain Kv or Kp (controller) not allowed");
                case 17687:
                    return QObject::tr("17687 - Proportional gain Pa (proportion) not allowed");
                case 17688:
                    return QObject::tr("17688 - Proportional gain Kv (velocity controller) not allowed");
                case 17689:
                    return QObject::tr("17689 - Reset time Tn (velocity controller) not allowed”");
                case 17690:
                    return QObject::tr("17690 - CONTROLERR_RANGE_ACCJUMPLIMITINGMODE");
                case 17691:
                    return QObject::tr("17691 - CONTROLERR_RANGE_ACCJUMPVALUE");
                case 17692:
                    return QObject::tr("17692 - CONTROLERR_RANGE_FILTERTIME");
                case 17693:
                    return QObject::tr("17693 - Dead zone not allowed");
                case 17696:
                    return QObject::tr("17696 - ”Rate time Tv (velocity controller) not allowed”");
                case 17697:
                    return QObject::tr("17697 - Damping time Td (velocity controller) not allowed");
                case 17698:
                    return QObject::tr("17698 - CONTROLERR_RANGE_IOUTPUTLIMIT");
                case 17699:
                    return QObject::tr("17699 - CONTROLERR_RANGE_DOUTPUTLIMIT");
                case 17700:
                    return QObject::tr("17700 - CONTROLERR_RANGE_POSIDISABLEWHENMOVING");
                case 17728:
                    return QObject::tr("17728 - Controller initialization");
                case 17729:
                    return QObject::tr("17729 - Axis address");
                case 17730:
                    return QObject::tr("17730 - Drive address");
                case 17744:
                    return QObject::tr("17744 - Following error monitoring (position)");
                case 17745:
                    return QObject::tr("17745 - Following error monitoring (velocity)");
                case 17824:
                    return QObject::tr("17824 - CONTROLERR_RANGE_AREA_ASIDE");
                case 17825:
                    return QObject::tr("17825 - CONTROLERR_RANGE_AREA_BSIDE");
                case 17826:
                    return QObject::tr("17826 - CONTROLERR_RANGE_QNENN");
                case 17827:
                    return QObject::tr("17827 - CONTROLERR_RANGE_PNENN");
                case 17828:
                    return QObject::tr("17828 - CONTROLERR_RANGE_AXISIDPRESP0");
                case 17920:
                    return QObject::tr("17920 - Drive ID not allowed");
                case 17921:
                    return QObject::tr("17921 - Drive type impermissible’");
                case 17922:
                    return QObject::tr("17922 - Drive operating mode impermissible’");
                case 17923:
                    return QObject::tr("17923 - Motor polarity inverted?");
                case 17924:
                    return QObject::tr("17924 - Drift compensation/speed offset (DAC offset)’");
                case 17925:
                    return QObject::tr("17925 - Reference speed (velocity pre-control)’");
                case 17926:
                    return QObject::tr("17926 - Reference output in percent’");
                case 17927:
                    return QObject::tr("17927 - Quadrant compensation factor’");
                case 17928:
                    return QObject::tr("17928 - Velocity reference point’");
                case 17929:
                    return QObject::tr("17929 - Output reference point’");
                case 17930:
                    return QObject::tr("17930 - Minimum or maximum output limits (output limitation)’");
                case 17931:
                    return QObject::tr("17931 - DRIVEERR_RANGE_MAXINCREMENT”");
                case 17932:
                    return QObject::tr("17932 - DRIVEERR_RANGE_ DRIVECONTROLDWORD”");
                case 17933:
                    return QObject::tr("17933 - DRIVEERR_RANGE_ RESETCYCLECOUNTER”");
                case 17935:
                    return QObject::tr("17935 - Drive torque output scaling impermissible’");
                case 17936:
                    return QObject::tr("17936 - Drive velocity output scaling is not allowed");
                case 17937:
                    return QObject::tr("17937 - Profi Drive DSC proportional gain Kpc (controller) impermissible’");
                case 17938:
                    return QObject::tr("17938 - Table ID is impermissible’");
                case 17939:
                    return QObject::tr("17939 - Table interpolation type is impermissible’");
                case 17940:
                    return QObject::tr("17940 - Output offset in percent is impermissible’");
                case 17941:
                    return QObject::tr("17941 - Profi Drive DSC scaling for calculation of “Xerr” (controller) impermissible’");
                case 17942:
                    return QObject::tr("17942 - Drive acceleration output scaling impermissible’");
                case 17943:
                    return QObject::tr("17943 - Drive position output scaling impermissible’");
                case 17948:
                    return QObject::tr("17948 - Drive filter type impermissible for command variable filter for the output position’");
                case 17949:
                    return QObject::tr("17949 - Drive filter time impermissible for command variable filter for the output position’");
                case 17950:
                    return QObject::tr("17950 - Drive filter order impermissible for command variable filter for the output position’");
                case 17952:
                    return QObject::tr("17952 - Bit mask for stepper motor cycle impermissible’");
                case 17953:
                    return QObject::tr("17953 - Bit mask for stepper motor holding current impermissible’");
                case 17954:
                    return QObject::tr("17954 - Scaling factor for actual torque (actual current) impermissible’");
                case 17955:
                    return QObject::tr("17955 - Filter time for actual torque is impermissible’");
                case 17956:
                    return QObject::tr("17956 - Filter time for the temporal derivation of the actual torque is impermissible’");
                case 17959:
                    return QObject::tr("17959 - DRIVEOPERATIONMODEBUSY");
                case 17968:
                    return QObject::tr("17968 - Overtemperature’");
                case 17969:
                    return QObject::tr("17969 - Undervoltage’");
                case 17970:
                    return QObject::tr("17970 - Wire break in phase A’");
                case 17971:
                    return QObject::tr("17971 - Wire break in phase B’");
                case 17972:
                    return QObject::tr("17972 - Overcurrent in phase A’");
                case 17973:
                    return QObject::tr("17973 - Overcurrent in phase B’");
                case 17974:
                    return QObject::tr("17974 - Torque overload (stall)’");
                case 17984:
                    return QObject::tr("17984 - Drive initialization’");
                case 17985:
                    return QObject::tr("17985 - Axis address’");
                case 17986:
                    return QObject::tr("17986 - Address IO input structure’");
                case 17987:
                    return QObject::tr("17987 - Address IO output structure’");
                case 18000:
                    return QObject::tr("18000 - Drive hardware not ready to operate’");
                case 18001:
                    return QObject::tr("18001 - Error in the cyclic communication of the drive (Life Counter)");
                case 18002:
                    return QObject::tr("18002 - Changing the table ID when active controller enable is impermissible’");
                case 18005:
                    return QObject::tr("18005 - Invalid IO data for more than ‘n’ continuous NC cycles’");
                case 18944:
                    return QObject::tr("18944 - Table ID not allowed");
                case 18945:
                    return QObject::tr("18945 - Table type not allowed");
                case 18946:
                    return QObject::tr("18946 - Number of lines in the table not allowed");
                case 18947:
                    return QObject::tr("18947 - Number of columns in the table is not allowed");
                case 18948:
                    return QObject::tr("18948 - Step size (position delta) not allowed");
                case 18949:
                    return QObject::tr("18949 - Period not allowed");
                case 18950:
                    return QObject::tr("18950 - Table is not monotonic");
                case 18951:
                    return QObject::tr("18951 - Table sub type is not allowed");
                case 18952:
                    return QObject::tr("18952 - Table interpolation type is not allowed");
                case 18953:
                    return QObject::tr("18953 - Incorrect table main type");
                case 18960:
                    return QObject::tr("18960 - Table initialization");
                case 18961:
                    return QObject::tr("18961 - Not enough memory");
                case 18962:
                    return QObject::tr("18962 - Function not executed, function not available");
                case 18963:
                    return QObject::tr("18963 - Line index not allowed");
                case 18964:
                    return QObject::tr("18964 - Column index not allowed");
                case 18965:
                    return QObject::tr("18965 - Number of lines not allowed");
                case 18966:
                    return QObject::tr("18966 - Number of columns not allowed");
                case 18967:
                    return QObject::tr("18967 - Error in scaling or in range entry");
                case 18968:
                    return QObject::tr("18968 - Multi table slave out of range");
                case 18969:
                    return QObject::tr("18969 - Solo table underflow");
                case 18970:
                    return QObject::tr("18970 - Solo table overflow");
                case 18971:
                    return QObject::tr("18971 - Incorrect execution mode");
                case 18972:
                    return QObject::tr("18972 - Impermissible parameter");
                case 18973:
                    return QObject::tr("18973 - Fifo is empty");
                case 18974:
                    return QObject::tr("18974 - Fifo is full");
                case 18975:
                    return QObject::tr("18975 - Point-Index of Motion Function invalid");
                case 18976:
                    return QObject::tr("18976 - No diagonalization of matrix");
                case 18977:
                    return QObject::tr("18977 - Number of spline points to less");
                case 18978:
                    return QObject::tr("18978 - Fifo must not be overwritten");
                case 18979:
                    return QObject::tr("18979 - Insufficient number of Motion Function points");
                case 19200:
                    return QObject::tr("19200 - Axis was stopped");
                case 19201:
                    return QObject::tr("19201 - Axis cannot be started");
                case 19202:
                    return QObject::tr("19202 - Control mode not permitted");
                case 19203:
                    return QObject::tr("19203 - Axis is not moving");
                case 19204:
                    return QObject::tr("19204 - Wrong mode for RestartPosAndVelo");
                case 19205:
                    return QObject::tr("19205 - Command not permitted");
                case 19206:
                    return QObject::tr("19206 - Parameter incorrect");
                case 19207:
                    return QObject::tr("19207 - Timeout axis function block");
                case 19208:
                    return QObject::tr("19208 - Axis is in protected mode");
                case 19209:
                    return QObject::tr("19209 - Axis is not ready");
                case 19210:
                    return QObject::tr("19210 - Error during referencing");
                case 19211:
                    return QObject::tr("19211 - Incorrect definition of the trigger input");
                case 19212:
                    return QObject::tr("19212 - Position latch was disabled");
                case 19213:
                    return QObject::tr("19213 - NC status feedback timeout’");
                case 19214:
                    return QObject::tr("19214 - Additional product not installed");
                case 19215:
                    return QObject::tr("19215 - No NC Cycle Counter Update");
                case 19216:
                    return QObject::tr("19216 - M-function query missing");
                case 19217:
                    return QObject::tr("19217 - Zero offset index is outside the range");
                case 19218:
                    return QObject::tr("19218 - R-parameter index or size is invalid");
                case 19219:
                    return QObject::tr("19219 - Index for tool description is invalid");
                case 19220:
                    return QObject::tr("19220 - Version of the cyclic channel interface does not match the requested function or the function block");
                case 19221:
                    return QObject::tr("19221 - Channel is not ready for the requested function");
                case 19222:
                    return QObject::tr("19222 - Requested function is not activated");
                case 19223:
                    return QObject::tr("19223 - Axis is already in another group");
                case 19224:
                    return QObject::tr("19224 - Block search could not be executed successfully");
                case 19225:
                    return QObject::tr("19225 - Invalid block search parameter");
                case 19232:
                    return QObject::tr("19232 - Cannot add all axes");
                case 19248:
                    return QObject::tr("19248 - Pointer is invalid");
                case 19249:
                    return QObject::tr("19249 - Memory size invalid");
                case 19250:
                    return QObject::tr("19250 - Cam table ID is invalid");
                case 19251:
                    return QObject::tr("19251 - Point ID is invalid");
                case 19252:
                    return QObject::tr("19252 - Number of points is invalid");
                case 19253:
                    return QObject::tr("19253 - MC table type is invalid");
                case 19254:
                    return QObject::tr("19254 - Number of rows invalid");
                case 19255:
                    return QObject::tr("19255 - Number of columns invalid");
                case 19256:
                    return QObject::tr("19256 - Step size invalid");
                case 19264:
                    return QObject::tr("19264 - Terminal type not supported");
                case 19265:
                    return QObject::tr("19265 - Register read/write error");
                case 19266:
                    return QObject::tr("19266 - Axis is enabled");
                case 19267:
                    return QObject::tr("19267 - Incorrect size of the compensation table");
                case 19269:
                    return QObject::tr("19269 - Not implemented");
                case 19296:
                    return QObject::tr("19296 - Motion command did not become active");
                case 19297:
                    return QObject::tr("19297 - Motion command could not be monitored by the PLC");
                case 19298:
                    return QObject::tr("19298 - Buffered command was terminated with an error");
                case 19299:
                    return QObject::tr("19299 - Buffered command was completed without feedback");
                case 19300:
                    return QObject::tr("19300 - 'BufferMode' is not supported by the command");
                case 19301:
                    return QObject::tr("19301 - Command number is zero");
                case 19302:
                    return QObject::tr("19302 - Function block was not called cyclically");
                case 19313:
                    return QObject::tr("19313 - Invalid NCI entry type");
                case 19314:
                    return QObject::tr("19314 - NCI feed table full");
                case 19316:
                    return QObject::tr("19316 - ST_NciTangentialFollowingDesc: tangential axis is not an auxiliary axis");
                case 19317:
                    return QObject::tr("19317 - ST_NciTangentialFollowingDesc");
                case 19318:
                    return QObject::tr("19318 - ST_NciTangentialFollwoingDesc");
                case 19319:
                    return QObject::tr("19319 - ST_NciGeoCirclePlane");
                case 19321:
                    return QObject::tr("19321 - Tangential following");
                case 19323:
                    return QObject::tr("19323 - Tangential following");
                case 19324:
                    return QObject::tr("19324 - FB_NciFeedTablePreparation");
                case 19325:
                    return QObject::tr("19325 - FB_NciFeedTablePreparation");
                case 19326:
                    return QObject::tr("19326 - ST_NciMFuncFast and ST_NciMFuncHsk");
                case 19327:
                    return QObject::tr("19327 - ST_NciDynOvr");
                case 19328:
                    return QObject::tr("19328 - ST_NciVertexSmoothing");
                case 19329:
                    return QObject::tr("19329 - FB_NciFeedTablePrepartion");
                case 19330:
                    return QObject::tr("19330 - ST_Nci");
                case 19344:
                    return QObject::tr("19344 - drive type");
                case 19345:
                    return QObject::tr("19345 - Direction");
                case 19346:
                    return QObject::tr("19346 - SwitchMode");
                case 19347:
                    return QObject::tr("19347 - Mode");
                case 19360:
                    return QObject::tr("19360 - KinGroup error");
                case 19361:
                    return QObject::tr("19361 - KinGroup timeout");
                case 19456:
                    return QObject::tr("19456 - Transformation failed");
                case 19457:
                    return QObject::tr("19457 - Ambiguous answer");
                case 19458:
                    return QObject::tr("19458 - Invalid axis position");
                case 19459:
                    return QObject::tr("19459 - Invalid dimension");
                case 19460:
                    return QObject::tr("19460 - NCERR_KINTRAFO_REGISTRATION");
                case 19461:
                    return QObject::tr("19461 - Newton iteration failed");
                case 19462:
                    return QObject::tr("19462 - Jacobi matrix cannot be inverted");
                case 19463:
                    return QObject::tr("19463 - Invalid cascade");
                case 19464:
                    return QObject::tr("19464 - Singularity");
                case 19467:
                    return QObject::tr("19467 - No metainfo");
                case 19488:
                    return QObject::tr("19488 - Transformation failed");
                case 19504:
                    return QObject::tr("19504 - Invalid input frame");
                case 19536:
                    return QObject::tr("19536 - Invalid offset");
                case 33056:
                    return QObject::tr("33056 - Invalid configuration for Object (e.g. in System Manager)");
                case 33057:
                    return QObject::tr("33057 - Invalid environment for Object (e.g. TcCom-Object's Hierarchy or missing/faulty Objects)");
                case 33058:
                    return QObject::tr("33058 - Incompatible Driver or Object");
                case 33072:
                    return QObject::tr("33072 - Invalid ObjectID of Communication Target");
                case 33073:
                    return QObject::tr("33073 - Communication Target expects Call in different Context");
                case 33074:
                    return QObject::tr("33074 - Invalid State of Communication Target");
                case 33076:
                    return QObject::tr("33076 - Communication with Communication Target cannot be established");
                case 33077:
                    return QObject::tr("33077 - Function Block Inputs are inconsitent");
                case 33083:
                    return QObject::tr("33083 - Transition Mode is invalid");
                case 33084:
                    return QObject::tr("33084 - BufferMode is invalid");
                case 33085:
                    return QObject::tr("33085 - Only one active Instance of Function Block per Group is allowed");
                case 33086:
                    return QObject::tr("33086 - Command is not allowed in current group state");
                case 33087:
                    return QObject::tr("33087 - Slave cannot synchronize");
                case 33088:
                    return QObject::tr("33088 - Invalid value for one or more of the dynamic parameters (Acceleration, Deceleration, Jerk)");
                case 33089:
                    return QObject::tr("33089 - IdentInGroup is invalid");
                case 33090:
                    return QObject::tr("33090 - The number of axes in the group is incompatible with the axes convention");
                case 33091:
                    return QObject::tr("33091 - Function Block or respective Command is not supported by Target");
                case 33093:
                    return QObject::tr("33093 - Mapping of Cyclic Interface between Nc and Plc missing (e.g. AXIS_REF, AXES_GROUP_REF, ...)");
                case 33094:
                    return QObject::tr("33094 - Invalid Velocity Value");
                case 33095:
                    return QObject::tr("33095 - Invalid Coordinate Dimension");
                case 33096:
                    return QObject::tr("33096 - Invalid Input Value");
                case 33097:
                    return QObject::tr("33097 - Unsupported Dynamics for selected Group Kernel");
                case 33098:
                    return QObject::tr("33098 - The programmed position dimension incompatible with the axes convention");
                case 33099:
                    return QObject::tr("33099 - Path buffer is invalid. E.g. because provided buffer has invalid address or is not big enough");
                case 33100:
                    return QObject::tr("33100 - Path does not contain any element");
                case 33101:
                    return QObject::tr("33101 - Provided Path buffer is too small to store more Path Elements");
                case 33102:
                    return QObject::tr("33102 - Dimension or at least one Value of Transition Parameters is invalid");
                case 33103:
                    return QObject::tr("33103 - Invalid or Incomplete Input Array");
                case 33104:
                    return QObject::tr("33104 - Path length is zero");
                case 33105:
                    return QObject::tr("33105 - Command is not allowed in current axis state");
                case 33106:
                    return QObject::tr("33106 - TwinCAT System is shutting down and cannot complete request");
                case 33107:
                    return QObject::tr("33107 - Configured axes convention and configured axes do not match");
                case 33108:
                    return QObject::tr("33108 - Invalid Number of ACS Axes");
                case 33109:
                    return QObject::tr("33109 - Invalid Number of MCS Data");
                case 33110:
                    return QObject::tr("33110 - Invalid Value Set for Kinematic Parameters");
                case 33112:
                    return QObject::tr("33112 - The Given ACS Values Cannot be Reached");
                case 33113:
                    return QObject::tr("33113 - The Set Target Positions Cannot be Reached");
                case 33120:
                    return QObject::tr("33120 - Circle Specification in Path is invalid");
                case 33121:
                    return QObject::tr("33121 - Maximum stream lines reached");
                case 33123:
                    return QObject::tr("33123 - Invalid First Segment");
                case 33124:
                    return QObject::tr("33124 - Invalid auxiliary point");
                case 33126:
                    return QObject::tr("33126 - Invalid parameter for GapControlMode");
                case 33127:
                    return QObject::tr("33127 - Group got unsupported Axis Event (e.g. State Change)");
                case 33128:
                    return QObject::tr("33128 - Unsupported Compensation Type");
                case 33130:
                    return QObject::tr("33130 - Invalid or no Tracking Transformation");
                case 33133:
                    return QObject::tr("33133 - Invalid Compensation ObjectId");
                case 33135:
                    return QObject::tr("33135 - Coupling would cause a cyclic dependency of axis (e.g. via MC_GearInPos)");
                case 33136:
                    return QObject::tr("33136 - Axis was not added to an axes group, the command is not valid");
                default:
                    return QString("%1 - %2").arg(errorCode).arg(QObject::tr("Unknown error number"));
            }
        }
    }

    namespace XTS
    {
        QString errorDescription(uint16_t errorCode)
        {
            switch (errorCode)
            {
            //Warning
            case 16641:
                return QObject::tr("16641 - Module overtemperature");
            case 16643:
                return QObject::tr("16643 - Undervoltage control voltage");
            case 17425:
                return QObject::tr("17425 - Undervoltage DC-Link");
            case 17426:
                return QObject::tr("17426 - Overvoltage DC-Link");
            case 17428:
                return QObject::tr("17428 - I2T Motor overload");
            //Error
            case 33027:
                return QObject::tr("33027 - Undervoltage control voltage");
            case 33028:
                return QObject::tr("33028 - Module overtemperature");
            case 33029:
                return QObject::tr("33029 - PD-Watchdog");
            case 33033:
                return QObject::tr("33033 - Overvolltage control voltage");
            case 33034:
                return QObject::tr("33034 - Peak overall backflow power threshold exceeded");
            case 33795:
                return QObject::tr("33795 - ADC Error");
            case 33796:
                return QObject::tr("33796 - Overcurrent Coil");
            case 33798:
                return QObject::tr("33798 - Undervoltage DC-Link");
            case 33799:
                return QObject::tr("33799 - Overvoltage DC-Link");
            case 33801:
                return QObject::tr("33801 - I2T - Motor module overload");
            case 33802:
                return QObject::tr("33802 - Overall current threshold exceeded");
            case 33803:
                return QObject::tr("33803 - Peak overall current threshold exceeded");
            case 33804:
                return QObject::tr("33804 - Peak module overall backflow current threshold exceeded");
            default:
                return QString("%1 - %2").arg(errorCode).arg(QObject::tr("Unknown error number"));
            }
        }
    }
}

#endif // BECKHOFFERRORCODES_H

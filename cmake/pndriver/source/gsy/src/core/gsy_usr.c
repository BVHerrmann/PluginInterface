/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
/*  This program is protected by German copyright law and international      */
/*  treaties. The use of this software including but not limited to its      */
/*  Source Code is subject to restrictions as agreed in the license          */
/*  agreement between you and Siemens.                                       */
/*  Copying or distribution is not allowed unless expressly permitted        */
/*  according to your license agreement with Siemens.                        */
/*****************************************************************************/
/*                                                                           */
/*  P r o j e c t         &P: PROFINET IO Runtime Software              :P&  */
/*                                                                           */
/*  P a c k a g e         &W: PROFINET IO Runtime Software              :W&  */
/*                                                                           */
/*  C o m p o n e n t     &C: GSY (Generic Sync Module)                 :C&  */
/*                                                                           */
/*  F i l e               &F: gsy_usr.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  User-Interface.                                                          */
/*                                                                           */
/*****************************************************************************/
#ifdef GSY_MESSAGE /**********************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Version                   Who  What                          */
/*  2004-11-29  P01.00.00.00_00.01.00.00 lrg  file creation                  */
/*  2005-04-04  P01.00.00.00_00.01.00.01 lrg  LSA-Trace                      */
/*  2005-05-06  P03.05.00.00_00.03.00.01 lrg  gsy_CloseChSys(): Beim Close-  */
/*              Channel Delaymessung stoppen                                 */
/*  2005-06-02  P03.06.00.00_00.03.00.01 lrg  Prototype for PT2              */
/*  2005-06-15  P03.06.00.00_00.03.02.01 lrg  OpenChannel trotz RxTxNanos=0  */
/*  2005-06-24  P03.06.00.00_00.03.02.02 lrg  neues Sync-Userinterface       */
/*  2005-07-26  P03.07.00.00_00.04.01.01 lrg  Mittelwert fuer Drift          */
/*  2005-09-14  P03.07.00.00_00.08.01.01 js   Initialisierung der neuen      */
/*                                            padding-felder (reserved) im   */
/*                                            SYNC-Frame                     */
/*                                            Initialisierung einiger        */
/*                                            Variablen um Compiler Warning  */
/*                                            zu verhindern.                 */
/*  2005-09-16  P03.07.00.00_00.08.01.01 js   ps (pico seconds) ergänzt.     */
/*  2005-09-16  P0x.0x.00.00_00.0x.0x.0x js   added brackets                 */
/*  2005-10-28  P03.08.00.00_00.01.01.01 lrg  Driftberechnung in TLVGetInfo  */
/*  2005-11-18  P03.08.00.00_00.01.02.01 lrg  181105lrg002: Master sendet    */
/*              nur bei Portstatus UP, CLOSED oder DISABLED                  */
/*  2005-11-22  P03.08.00.00_00.01.02.02 lrg  -GSY_OPC_SYNC_RECORD_READ_REAL */
/*  2005-12-01  P03.08.00.00_00.01.02.03 lrg  Neu: Diagnose-Indication       */
/*              gsy_SyncDataLoad() SyncSendFactor auf 31,25µs und Timeout-   */
/*              Factor auf Vielfaches von SyncSendFactor umgestellt          */
/*  2005-12-20  P03.08.00.00_00.01.02.04 lrg  gsy_SyncUserInd()              */
/*              pChUsr->RTC2Active weiterschalten                            */
/*  2005-12-22  P03.09.00.00_00.01.01.02 lrg  gsy_SyncUserInd() falls kein   */
/*              lower RQB fuer die Bandbreitenaktivierung da ist, Funktion   */
/*              nachtraeglich in gsy_Rtc2Cnf() ausfuehren                    */
/*              GSY_SYNC_ADJUST() mit negativem Ausgangswert von GSY_SYNC_   */
/*              ALGORITHM() versorgen                                        */
/*  2006-02-08  P03.09.00.00_00.03.01.02 lrg  ArtsRQ AP00280814 RTC2-Band-   */
/*              breiten reservierung in gsy_SyncStop() deaktivieren          */
/*              gsy_SlaveSet() benutzt GSY_GREQ_UINT64() statt >=            */
/*              gsy_SyncDataLoad(): SyncInterval = SendInterval              */
/*              gsy_SyncDataLoad(): SyncInTime = SendInterval*TimeoutFactor  */
/*              gsy_MasterStart(): Clockparameter generieren                 */
/*              gsy_SlaveStart(): SyncInTime und SyncOutTime generieren      */
/*              gsy_SlaveAdjust(): Bei WindowSize 0 immer synchron           */
/*  2006-03-08  P03.10.00.00_00.01.01.02 lrg  Generalisierte CM-PD-Schnitt-  */
/*              stelle mit Opcodes GSY_OPC_PRM_...                           */
/*  2006-03-20  P03.10.00.00_00.01.01.03 lrg  GSY_OPC_CHANNEL_INFO_GET       */
/*  2006-04-12  P03.11.00.00_00.01.01.03 lrg  gsy_PrmCommit():               */
/*              Indication-Flags/Event zurücksetzen (ArtsRQ00303293)         */
/*  2006-04-26  P03.11.00.00_00.01.01.04 lrg  "neue" Sync-Schnitts. entfernt */
/*              GSY_OPC_CHANNEL_INFO_GET bei EDD o. Zeitstempel ermöglichen  */
/*  2006-05-03  P03.11.00.00_00.02.01.02 lrg  030506lrg001: Master-Sync-     */
/*              indication als leere Indication hochgeben                    */
/*  2006-05-10  P03.11.00.00_00.02.01.03 lrg  Morphing nach PrmCommit unter- */
/*              stützt ->  gsy_PrmCommitNext()                               */
/*              PREPARE, END, COMMIT bei EDD o. Zeitstempel ermöglichen      */
/*  2006-05-10  P03.11.00.00_00.03.01.02 lrg  Port-MAC-Adresse benutzen in:  */
/*              gsy_MasterSend() und  gsy_MasterSyncCnf()                    */
/*  2006-05-10  P03.11.00.00_00.03.01.03 lrg  gsy_PrmCommitNext(): keine     */
/*              SyncIndication bei unveraenderter GSY_SYNC_ROLE_LOCAL        */
/*  2006-05-30  P03.11.00.00_00.04.01.02 lrg  erweitert: GSY_DIAG_DATA_BLOCK */
/*              SeqIdSnd und SeqIdCnf von .Port[] nach .Master verschoben    */
/*              300506lrg001: Versuch einer genaueren Taktsynchronisation    */
/*  2006-06-01  P03.11.00.00_00.04.02.03 lrg  DiagTrace vor gsy_SlaveSet()   */
/*              GSY_OPC_PRM_INDICATION_PROVIDE bei EDD o. Zeitstempel ermögl.*/
/*              010606lrg001: Diff. zwischen Get-Zeitstempel Empfangszeit-   */
/*              stempel mit Stempelueberlauf berechnen                       */
/*  2006-06-02  P03.11.00.00_00.04.03.02 lrg  GSY_SYNC_ADJUST() ausgelagert  */
/*              in das Makro GSY_SYNC_ALGORITHM().                           */
/*              GSY_OLD_IF; alte "neue" Sync-Schnitts. entfernt              */
/*              CurrentUTCOffset = 33                                        */
/*              Pruefung von Padding1, Slot und Subslot in gsy_PrmWrite()    */
/*  2006-06-13  P03.12.00.00_00.01.01.02 130606lrg002: Diagnoserecord nicht  */
/*              nur bei Aenderungen schreiben. 130606lrg003: neues Ausgangs- */
/*              makro: GSY_GET_SENDCLOCK(). 130606lrg004: gsy_SlaveAction()  */
/*              neu von Ines.                                                */
/*  2006-06-27  P03.12.00.00_00.02.01.02 lrg: gsy_SlaveSet() benutzt         */
/*              GSY_LOWER_RQB_CYCLE_RCF_INTERVAL_SET(), Basisindex 0xb000    */
/*              plus SyncId fuer SyncLogbook, 280606lrg001 SET im State WAIT */
/*              P- und I-Reglewerte von GSY_SYNC_ALGORITHM() in SyncLogbook  */
/*              GSY_FILE_SYSTEM_EXTENSION()                                  */
/*  2006-07-07  P03.12.00.00_00.25.01.02 lrg: gsy_SlaveAction() RCF-         */
/*              Korrektur bei Offsetberechnung und lokale Zeit auf Empfangs- */
/*              zeitpunkt des Syncframes verschieben. 060706lrg001: Master   */
/*              sendet nur ueber Ports, an denen schon Delay gemessen wurde  */ 
/*              070706lrg001: FirstIndication nach PrmCommit ohne PrmWrite   */
/*  2006-07-26  P03.12.00.00_00.25.02.02 170706lrg001: gsy_SlaveAction()     */
/*              170706lrg002: GSY_SLAVE_STATE_FIRST. Arts-RQ347146:          */
/*              gsy_PrmEventSet() ChannelProperties nicht mehr vorbelegen.   */
/*              190706lrg001: Lower RQBs fuer GSY_SYNC_SET() und die RTC2-   */
/*              Bandbreitensteuerung in gsy_ChOpen() statt gsy_PrmEnd() all. */
/*              GSY_UINT64 -> LSA_UINT64. GSY_LOWER_TIME_SET --> PTCP-Time.  */
/*              250706lrg001: bei Fehler von gsy_TimerClean: erneut Freigabe */
/*              250706lrg002: in gsy_MasterStart() Delaymessung neu starten  */
/*              260706lrg001: gsyPrmWrite() lehnt GSY_SYNC_ROLE_LOCAL ab     */
/*              und prueft Stratum nur bei GSY_SYNC_ROLE_MASTER              */
/*              270706lrg001: immer GSY_SYNC_EVENT_NO_MESSAGE_RECEIVED melden*/
/*              270706lrg002: Fehler im GNU-Compiler (0-)                    */
/*              gsy_PrmRead() an akt. Spec angepasst (ArtsRQ343060,343054)   */
/*  2006-08-03  P03.12.00.00_00.25.04.01 js Vergleich mit SyncID und nicht   */
/*                                          pChUsr->SyncId (040806js001)     */
/*                                          Avoid compiler warining with gcc */
/*              090806js001 : SyncEvents müssen auch indication erzeugen.    */
/*                            Alle anderen werden nicht mehr vermerkt.       */ 
/*              110806js001 : Nach commit Aktivierung der geänderten         */
/*                            bandbreite.                                    */ 
/*              110806js001 : Änderung der SubdomainUUID bewirkt Neustart    */
/*                            der Synchronisation.                           */ 
/*              240806lrg002: RQB-Zaehler fuer RTC2-RQBs: RTC2RblCount       */
/*                            Neu: gsy_SlaveStop()                           */ 
/*  2006-09-04  P03.12.00.00_00.25.04.02 lrg gsy_usr.c aufgeteilt in gsy_usr */
/*              .c gsy_msm.c (MasterSlaveManager Procs:gsy_Slave*()) und     */ 
/*              gsy_bma.c (BestMasterAlgorithm Procs:gsyMaster*())           */ 
/*              Neu: gsy_PrmWriteClockCheck()                                */ 
/*  2006-10-04  P03.12.00.00_00.30.01.02 041006lrg001: GSY_SYNC_EVENT_NONE   */
/*              beliebig oft hochgeben (ArtsRQ 371329)                       */
/*  2006-10-18  P03.12.00.00_00.30.02.02 lrg GSY_OPC_PRM_WRITE und GSY_OPC_  */
/*              PRM_READ liefern GSY_RSP_ERR_PRM_INDEX bei RxTxNanos == 0    */
/*              gsy_DiagUserInd(): 0 auch für Master(nano)seconds bei GSY_   */
/*              DIAG_SOURCE_OFFSET zulassen.                                 */
/*              gsy_PrmCommitNext() geht in "FATAL_ERROR" wenn kein lower    */
/*              RQB allokiert werden kann oder wenn GSY_SYNC_GET() Fehler    */
/*              bei der eingestellten SyncId meldet.                         */
/*              gsy_PrmCommit() setzt RTC2-Intervall ohne vorheriges loesch. */
/*  2006-10-18  P03.12.00.00_00.30.03.02 191006lrg002: MAC-Adresse immer in  */
/*              Diagnosepuffer schreiben                                     */
/*              291006lrg001: Rtc2Cnf-Response fuer Callback auf OK setzen   */
/*  2006-11-09  P03.12.00.00_00.30.04.02 gsy_PrmWrite(): PDPortDataAdjust-   */
/*              Record uebernehmen und SendClockRecord ignorieren            */ 
/*              gsy_PrmCommitNext(): neue Fkt. gsy_PrmCommitEnd() aufrufen   */ 
/*              gsy_PrmRead(): PDPortDataAdjust-Record auslesen              */ 
/*  2006-11-16  P03.12.00.00_00.30.04.03 161106js001 : PortDataAdjust:       */
/*              DomainBoundary Defaultwert wenn leerer record. Leerer Record */
/*              erlaubt (kein Fehler) Fehlertrace wenn Blockfehler ergänzt.  */ 
/*              Nach commit Defaultwerte vorbelegen, da nicht unbedingt ein  */ 
/*              Prepare kommmt. A/B Puffermodell nicht korrekt umgesetzt     */ 
/*              damit Lesen des records aus falschen Puffer. Validierung     */ 
/*              der subblocklength ergänzt. Zugriff auf Blocktype nur wenn   */ 
/*              dieser nicht ausserhalb des records liegt.                   */ 
/*  2006-11-20  P03.12.00.00_00.40.01.02 lrg: Aufruf von gsy_PrmPrepare()    */
/*              in gsy_PrmCommitEnd() fuer Parametrierung ohne PREPARE.      */
/*              gsy_DiagUserGet() und gsy_DiagUserInd() ueberarbeitet.       */
/*  2006-11-29  P03.12.00.00_00.40.02.02 291106lrg001: Vermeiden einer       */
/*              Timeout-PRM-Indication nach Beenden eines Dienstes           */
/*  2006-11-30  P03.12.00.00_00.40.03.02 301106lrg001: ArtsRQ AP00391119     */
/*              Forwarding u. Delaymessung nicht ohne Zeitstempelung         */
/*              301106lrg002: ArtsRQ AP00391256 RecordIndex ist LSA_UINT32   */
/*              301106lrg003: GSY_PRM_STATE_WRITE bei SendClockRecord setzen */
/*  2006-12-06  P03.12.00.00_00.40.04.02 051206lrg001: SendClockRecord aus-  */
/*              werten und beim PrmEnd gegen SendClockFactor aus SyncRecord  */
/*              pruefen. Kein Aufruf mehr an EDD zum ReserverInterval-Check  */
/*              061206lrg001 "neue" Diagnosequellen nicht in DiagRecord      */
/*  2006-12-13  P03.12.00.00_00.40.05.02 131206lrg001:  ArtsRQ AP00395885    */
/*              SendClockRecord auch bei EDD ohne Zeitstempelung schreibbar  */
/*              131206lrg002: ReservedIntervalEnd mit SendClock pruefen      */
/*              151206lrg001: GSY_OPC_DELAY_CTRL, GSY_OPC_FWD_CTRL entfernt  */
/*  2006-12-20  P03.12.00.00_00.40.06.02 201206lrg001: Diagnosemeldung beim  */
/*              Wechsel RTA <-> RTC unterdruecken. 150107lrg001: Bei Wechsel */
/*              der Subdomain: Driftmessung zu neuem Master ermoeglichen.    */
/*              160107lrg001: Stratum immer = 0 und ClockRole nur noch       */
/*              PRIMARY oder SECONDARY projektiert durch SyncClass           */
/*              170107lrg001: Arts-RQ AP00404386: bei EDD ohne Zeitstempelung*/
/*              darf der PortDataAdjustRecord geschrieben werden aber wenn   */
/*              ein DomainBoundary Subblock enthalten ist, wird              */
/*              GSY_RSP_ERR_PRM_BLOCK zurückgegeben                          */
/*  2007-01-23  P03.12.00.00_00.40.07.02 lrg GSY_PRM_TRACE, 220107lrg002     */
/*              230107lrg001: Nur von GSY_SLAVE_STATE_WAIT_SET auf _SET      */
/*  2007-01-29  P03.12.00.00_00.40.08.02 290107lrg001: PDSyncData V1.1       */
/*              mit SubdomainName                                            */
/*  2007-02-12  P03.12.00.00_00.40.09.02 120207lrg001: QueueEvent:           */
/*              PrmIndications ohne Rbl konnten welche mit Rbl ueberholen    */
/*  2007-02-26  P03.12.00.00_00.40.10.02 260207lrg001: Arts-RQ AP00425128:   */
/*              "NotSync gehend" vor "Timeout kommend" melden                */
/*  2007-05-03  P03.12.00.00_00.40.11.02 030507lrg002: Arts-RQ AP00456200:   */
/*              MAC-Adresse bei synchronem Slave aus der Slave-Struktur      */
/*              wegen gleichzeitigem Senden von mehreren prim. Mastern       */
/*  2007-07-25  P04.00.00.00_00.30.01.00 lrg: RTSync, PeerToPeerBoundary     */
/*              ***TODO: PrmRead PeerToPeerBoundary                          */
/*  2007-08-06  P04.00.00.00_00.30.02.00 030807JS: RTC2Sync gibt es nicht    */
/*              mehr. Setzen des Sync-State im EDD (erster hack)             */
/*  2007-08-21  P04.00.00.00_00.30.03.02 210807lrg001: SyncTimeout = Zaehler */
/*              Sync/SendInterval sind Vielfache von 10ms (30...39 => 30)    */
/*  2007-08-23  P04.00.00.00_00.30.03.03 230807lrg001: Ueberholen von PRM-   */
/*              Indicaton-RQBs verhindern (Arts: AP00507560)                 */
/*              230807lrg002: Abschalten des lower SyncState bei STOP        */
/*  2007-08-29  P04.00.00.00_00.30.04.02 290807lrg001: MasterStartupTime     */
/*              PortDataAdjust.SubblockDomainBoundary.VersionLow = 1:        */
/*  2007-09-04  P04.00.00.00_00.30.05.02 040907lrg001: Sync-Meldung an EDD   */
/*  2007-09-19  P04.00.00.00_00.30.06.02 190907lrg001: Master und Slave bei  */
/*              entsprechendem State in gsy_Rtc2Cnf() beenden.               */
/*              210907lrg001: GSY_CTRL_RTC2 und GSY_SET_SYNC_USER entfernt   */
/*              #define GSY_OLD_IF, GSY_SYNC_DATA_MASTER_STARTUP entfernt    */ 
/*  2007-10-04  P04.00.00.00_00.40.01.02 041007lrg002:  Announce nur an      */
/*              Master wenn er laeuft. 041007lrg004: fehlendes "else" ...    */
/*              AP00526517: RateValid, PrimaryMaster                         */
/*  2007-11-16  P04.00.00.00_00.50.01.02 161107lrg003: Todo: nur bei SyncId  */
/*              = Clock DefaultBoundary setzen sonst bisherige Konfiguration */
/*              uebernehmen.                                                 */
/*  2007-11-22  P04.00.00.00_00.60.01.02 lrg: gsy_PrmValidName() ueberprueft */
/*              SubdomainName auf Laenge und Syntax (wie DCP-StationName)    */
/*  2008-01-07  P04.00.00.00_00.60.02.02 070108lrg003: Diag.Subdomain        */
/*  2008-01-25  P04.00.00.00_00.70.02.02 250108lrg001: Subdomain beim        */
/*              Empfang von Announce pruefen                                 */
/*  2008-03-20  P04.00.00.00_00.80.01.02 100308lrg001: MasterMAC beim        */
/*              Empfang von Announce pruefen                                 */
/*              200308lrg001: Update running Master, 170408lrg001:           */
/*              Priority1.Level 170408lrg002: RcvSyncPrio,LocalPrio          */
/*  2008-06-10  P04.00.00.00_00.80.04.02 190608lrg001: Arts-RQ AP00656763    */
/*              Beim Standard-EDD den PortDataAdjustRecord mit PeerToPeer-   */
/*              Boundary nur bei gesetzter PTCP-Boundary (Bit 1) ablehnen    */
/*  2008-06-20  P04.00.00.00_00.80.05.02 lrg: GSY_MESSAGE                    */
/*  2008-07-25  P04.01.00.00_00.01.02.03 250708lrg001: InfoSync              */
/*  2008-10-13  P04.01.00.00_00.01.03.02 131008lrg001: Casts for C++         */
/*  2008-10-17  P04.01.00.00_00.02.02.02 GSY_HW_TYPE stett EDD_HW_TYPE_USED  */
/*  2008-11-28  P04.01.00.00_00.02.02.03 KRISC32-Unterstuetzung              */
/*  2008-12-04  P04.01.00.00_00.02.03.01 lrg: Package Version (PNIO)         */
/*  2008-12-10  P04.01.00.00_00.02.03.02 101208lrg001: Announce mit          */
/*              gsy_DataRecv() empfangen                                     */
/*  2009-02-10  P04.01.00.00_00.03.01.02 100209lrg001:                       */
/*              Nicht auf EDDP-SyncChannel die Zeit holen                    */
/*  2009-04-08  P04.01.00.00_00.03.03.02 lrg: K32_RQB_TYPE_SLAVE_REQ         */
/*  2009-04-24  P04.01.00.00_00.03.04.02 240409lrg002: no master in KRISC32  */
/*  2009-08-21  P04.02.00.00_00.01.04.02 240409lrg002 removed                */
/*  2009-09-28  P04.02.00.00_00.01.04.03 240409lrg002 changed to check for   */
/*              HW master or SyncSend supportet by EDD                       */
/*  2009-10-08  P04.02.00.00_00.01.04.04 lrg: GSY_HW_TYPE_SLAVE_MSK...       */
/*  2009-11-13  P05.00.00.00_00.01.01.02 lrg: GSY_LOWER_SLAVE/MASTER_UPDATE  */
/*              171109lrg001: check for PDU from myself in gsy_TLVGetInfo()  */
/*              not longer in gsy_FwdFrameUserForward()                      */
/*  2010-03-03  P05.00.00.00_00.01.03.02 lrg: Announce channel               */
/*  2010-03-31  P05.00.00.00_00.01.04.02 300310lrg001: Arts-RQ AP00933526    */
/*              Check Boundaries for supported SyncIds                       */
/*  2010-07-16  P05.00.00.00_00.01.06.02 lrg: AP00999032 _REQUEST_UPPER_DONE */
/*              German comments translated to english                        */
/*  2010-10-08  P05.00.00.00_00.01.08.03 230910lrg001: PDSyncPLL record      */
/*  2010-11-30  P05.00.00.00_00.02.01.02 301110lrg001:                       */
/*              Indicate slave Takeover and Timeout to master                */
/*  2010-12-13  P05.00.00.00_00.02.01.03 131210lrg001: PDSyncPLL-Record solo */
/*  2010-12-22  P05.00.00.00_00.03.12.02 lrg: AP01017485 Index-Trace         */
/*  2011-05-16  P05.00.00.00_00.05.21.02 160511lrg: AP01183082 gsy_SlaveTimer*/
/*  2011-05-27  P05.00.00.00_00.05.21.02 270511lrg: Trace (%d)               */
/*  2011-05-31  P05.00.00.00_00.05.21.02 310511lrg: AP01178714: SetWaitCount */
/*  2011-09-27  P05.00.00.00_00.05.56.02 AP01191740 Trace parameter count    */
/*  2012-05-31  P05.00.00.00_00.05.97.02 AP01361288 RTC2 - Fremdlast, Fatal  */
/*              No FATAL_ERROR on GSY_SYNC_GET RetVal != LSA_RET_OK          */
/*  2012-06-11  P05.00.00.00_00.05.97.02 AP01368288 Compiler Warnings        */
/*  2012-09-20  P05.02.00.00_00.04.04.02 AP01382171 Compiler Warnings        */
/*  2013-03-20  P05.00.00.00_00.05.97.02 AP01512559: check SyncId of user    */
/*              channel not used by another user channel                     */
/*  2014-05-20  P06.00.00.00_00. TODO gl 966426: [GSY] Die Synchronisation  */
/*              über EDDL und i210 soll ermöglicht werden                    */
/*  2014-09-12  P06.00.00.00_00.01.44.01 1092865 PDEV-Prm, Erweiterung von   */
/*              PrmWrite und PrmEnd um Fehler-Informationen                  */
/*  2014-11-05  P06.00.00.00_00.01.49.01 1126470 [GSY] Announce-MAC-Adressen */
/*              nur bei Masterprojektierung aktivieren                       */
/*  2015-04-15  V06.00, Inc04, Feature 1204141, Task 1234427                 */
/*              GSY_OPC_MASTER_CONTROL added                                 */
/*  2015-05-11  V06.00, Inc05, Feature 1227823, Task 1311090                 */
/*              only one user channel for clock or time sync                 */
/*              GSY_OPC_CHANNEL_INFO_GET removed                             */
/*  2015-06-26  V06.00, Inc05, Request 1365226: check pChUsr for NULL        */
/*  2015-07-17  V06.00, Inc07, Feature 1204141, Task 1366974                 */
/*              Timesync Diagnosen richtig an CM geben                       */
/*  2015-10-02  V06.00, Inc10, RQ 1460015: gsy_PrmCommitEndDiag()            */
/*  2015-12-10  V06.00, Inc12, RQ 1533977: gsy_PrmWriteSyncPLL()             */
/*  2017-01-10  V06.00, RQ 2049154: gsy_PrmCommitEnd():                      */
/*              Do not use current boundary configuration if no              */
/*              PortDataAdjust record been written.                          */
/*                                                                           */
/*****************************************************************************/
#endif /* GSY_MESSAGE */

/*===========================================================================*/
/*                                 module-id                                 */
/*===========================================================================*/
#define LTRC_ACT_MODUL_ID 	2		/* GSY_MODULE_ID_USR */
#define GSY_MODULE_ID 		LTRC_ACT_MODUL_ID 

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/
#include "gsy_inc.h"
#include "gsy_int.h"
GSY_FILE_SYSTEM_EXTENSION(GSY_MODULE_ID)

/*****************************************************************************/
/* Local Data                                                                */
/* ...                                                                       */
/*****************************************************************************/

/*****************************************************************************/
/* Internal function: gsy_ChOpen()                                           */
/*****************************************************************************/
LSA_UINT16  GSY_LOCAL_FCT_ATTR  gsy_ChOpen(
GSY_UPPER_RQB_PTR_TYPE  pRbu)
{
	LSA_SYS_PATH_TYPE		SysPath;
	GSY_DETAIL_PTR_TYPE		pCDB;
	LSA_SYS_PTR_TYPE		pSys;
	GSY_LOWER_RQB_PTR_TYPE	pRbl = LSA_NULL;
	GSY_CH_SYS_PTR 	        pChSys = LSA_NULL;
	GSY_CH_USR_PTR 	        pChUsr = LSA_NULL;
	GSY_CH_ANNO_PTR	        pChAnno = LSA_NULL;
	LSA_UINT16   	        RespVal = GSY_RSP_ERR_RESOURCE;
	LSA_UINT16   	        RetVal  = LSA_RET_OK;

	GSY_FUNCTION_TRACE_01(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,"  > gsy_ChOpen() pRbu=%X",
					pRbu);

	SysPath = GSY_UPPER_RQB_SYSPATH_GET(pRbu);
    GSY_GET_PATH_INFO(&RetVal, &pSys, &pCDB, SysPath);
    if (RetVal == LSA_RET_OK)
    {
		if (pCDB->PathType == GSY_PATH_TYPE_SYNC)
		{
			/* System channel: already exists another one with the same NicId?
			*/
			pChSys = gsy_ChSysFind(pCDB->NicId);
		    if (!LSA_HOST_PTR_ARE_EQUAL(pChSys, LSA_NULL))
			{
				GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_ChOpen() System channel pChSys=%X already exists with pCDB->NicId=%u",
								pChSys, pCDB->NicId);
				pChSys = LSA_NULL;
				RespVal = GSY_RSP_ERR_CONFIG;
			}
			else
		    {
				/* Allocate strukture for system channel
				*/
			    pChSys = gsy_ChSysAlloc(pRbu, pSys, pCDB);
			    if(!LSA_HOST_PTR_ARE_EQUAL(pChSys, LSA_NULL))
			    {
					/* Queue RQB and open lower channel
					*/
					gsy_PutUpperQ(&pChSys->QueueUpper, pRbu);
		            RespVal = gsy_OpenLowerStart(pChSys, LSA_FALSE);
			        if (RespVal != GSY_RSP_OK_ACTIVE)
						gsy_GetUpperQ(&pChSys->QueueUpper, pRbu);
			    }
		    }

			/* On error: free Pathinfo and channel
			*/
	        if ((RespVal != GSY_RSP_OK) && (RespVal != GSY_RSP_OK_ACTIVE))
	        {
			    if (!LSA_HOST_PTR_ARE_EQUAL(pChSys, LSA_NULL))
				{
					RetVal = gsy_ChSysClean(pChSys);
					if (LSA_RET_OK != RetVal)
					{
						GSY_ERROR_TRACE_02(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_FATAL,"*** gsy_ChOpen() Cannot clean sys channel pChSys=%X RetVal=0x%x",
									pChSys, RetVal);
					    gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FREE_MEM, pCDB, 0);
					}
				}
				/* 210710lrg001: Callback before RELEASE_PATH_INFO()
				*/ 
		        gsy_CallbackRbu(pSys, pRbu, RespVal);
			    if (!LSA_HOST_PTR_ARE_EQUAL(pChSys, LSA_NULL))
				{
					gsy_ChSysFree(pChSys);
				}
	        }
		}
		else if (pCDB->PathType == GSY_PATH_TYPE_ANNO)
		{
			/* Announce channel: Exists a system channel with this NicId?
			*/
			pChSys = gsy_ChSysFind(pCDB->NicId);
		    if (LSA_HOST_PTR_ARE_EQUAL(pChSys, LSA_NULL))
			{
				GSY_ERROR_TRACE_01(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_ERROR,"*** gsy_ChOpen() No system channel found with pCDB->NicId=%u",
								pCDB->NicId);
				RespVal = GSY_RSP_ERR_SEQUENCE;
			}
		    else if (GSY_CHA_STATE_OPEN != pChSys->State)
			{
				GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_ChOpen() System channel not open: State=0x%x pCDB->NicId=%u",
								pChSys->State, pCDB->NicId);
				RespVal = GSY_RSP_ERR_SEQUENCE;
			}
		    else if (!LSA_HOST_PTR_ARE_EQUAL(pChSys->pChAnno, LSA_NULL))
			{
				GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_ChOpen() Announce channel pChAnno=%X already exists with pCDB->NicId=%u",
								pChSys->pChAnno, pCDB->NicId);
				RespVal = GSY_RSP_ERR_SEQUENCE;
			}
			else
		    {
				/* Allocate strukture for announce channel
				*/
			    pChAnno = gsy_ChAnnoAlloc(pChSys, pRbu, pSys, pCDB);
			    if(!LSA_HOST_PTR_ARE_EQUAL(pChAnno, LSA_NULL))
			    {
					/* Queue RQB and open lower channel
					*/
					gsy_PutUpperQ(&pChAnno->QueueUpper, pRbu);
		            RespVal = gsy_OpenLowerStart(pChSys, LSA_TRUE);
			        if (RespVal != GSY_RSP_OK_ACTIVE)
						gsy_GetUpperQ(&pChAnno->QueueUpper, pRbu);
			    }
		    }

			/* On error: free path info and channel structure
			*/
	        if ((RespVal != GSY_RSP_OK) && (RespVal != GSY_RSP_OK_ACTIVE))
	        {
			    if (!LSA_HOST_PTR_ARE_EQUAL(pChAnno, LSA_NULL))
				{
					gsy_ChAnnoClean(pChAnno);
				}
				/* 210710lrg001: Callback before RELEASE_PATH_INFO()
				*/ 
		        gsy_CallbackRbu(pSys, pRbu, RespVal);
			    if (!LSA_HOST_PTR_ARE_EQUAL(pChAnno, LSA_NULL))
				{
					gsy_ChAnnoFree(pChAnno);
				}
	        }
		}
		else if (pCDB->PathType == GSY_PATH_TYPE_USR)
		{
			/* User channel: exists a system channel with this NicId?
			*/
			pChSys = gsy_ChSysFind(pCDB->NicId);
		    if (LSA_HOST_PTR_ARE_EQUAL(pChSys, LSA_NULL))
			{
				GSY_ERROR_TRACE_01(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_ERROR,"*** gsy_ChOpen() No system channel found with pCDB->NicId=%u",
								pCDB->NicId);
				RespVal = GSY_RSP_ERR_SEQUENCE;
			}
		    else if (LSA_HOST_PTR_ARE_EQUAL(pChSys->pChAnno, LSA_NULL))
			{
				GSY_ERROR_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_ChOpen() No announce channel found with pCDB->NicId=%u",
								pCDB->NicId);
				RespVal = GSY_RSP_ERR_SEQUENCE;
			}
		    else if (!LSA_HOST_PTR_ARE_EQUAL(pChSys->pChUsr, LSA_NULL))
			{
				GSY_ERROR_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_ChOpen() User channel for pChUsr=%X already open",
								pChSys->pChUsr);
				RespVal = GSY_RSP_ERR_CONFIG;
			}
			else
		    {
				/* Allocate strukture for user channel
				*/
			    pChUsr = gsy_ChUsrAlloc(pChSys, pRbu, pSys, pCDB);
			    if(!LSA_HOST_PTR_ARE_EQUAL(pChUsr, LSA_NULL))
			    {
					/* Connect system channel with this user channel
					*/
					pChSys->pChUsr = pChUsr;
					pChSys->UserCount++;

					if (GSY_HW_TYPE_SLAVE_SW == (pChSys->HardwareType & GSY_HW_TYPE_SLAVE_MSK))
					{
						/* 190706lrg001: allocate lower RQBs for 
						 * GSY_SYNC_SET() and RTClass2 bandwidth control
						*/
						GSY_SYNC_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "  - gsy_ChOpen() Allocating 2 sync set RQBs");
						RetVal = gsy_AllocEthParam(pChSys, &pChUsr->pSyncSetRbl, LSA_FALSE);
						if (LSA_RET_OK == RetVal)
						{
							RetVal = gsy_AllocEthParam(pChSys, &pRbl, LSA_FALSE);
							if (LSA_RET_OK == RetVal)
							{
								/*  Store lower RTC2-RQB in channel structure
								 *  240806lrg002; and increment RQB counter
								*/
								gsy_EnqueLower(&pChUsr->qRTC2Rbl,pRbl);
								pChUsr->RTC2RblCount++;
							}
						}
						if (LSA_RET_OK != RetVal)
						{
							RespVal = GSY_RSP_ERR_RESOURCE;
							GSY_ERROR_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_ChOpen() Cannot allocate lower RTC2-RQB, RespVal=0x%X", RespVal);
						}
					}
					/* Prepare User channel for PrmWrite without PrmPrepare
					*/
					RespVal = gsy_PrmPrepare(pChUsr, LSA_NULL);
			    }
		    }
	        if (RespVal != GSY_RSP_OK)
			{
				/* 210710lrg001: free allocated user channel structure on error
				*/ 
			    if (!LSA_HOST_PTR_ARE_EQUAL(pChUsr, LSA_NULL))
				{
					gsy_ChUsrClean(pChUsr);
				}
				/* 210710lrg001: Callback before RELEASE_PATH_INFO()
				*/ 
		        gsy_CallbackRbu(pSys, pRbu, RespVal);
			    if (!LSA_HOST_PTR_ARE_EQUAL(pChUsr, LSA_NULL))
				{
					gsy_ChUsrFree(pChUsr);
				}
			}
		    else
		    {
		        gsy_CallbackUsr(pChUsr, pRbu, RespVal);
		    }
		}
		else
		{
			GSY_ERROR_TRACE_01(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_ERROR,"*** gsy_ChOpen() invalid pCDB->PathType=%u",
								pCDB->PathType);

			RespVal = GSY_RSP_ERR_CONFIG;

			/* 210710lrg001: Callback and RELEASE_PATH_INFO()
			*/ 
	        gsy_CallbackRbu(pSys, pRbu, RespVal);
		    GSY_RELEASE_PATH_INFO(&RetVal, pSys, pCDB);
		}
    }
    else
    {
		GSY_ERROR_TRACE_01(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_ERROR,"*** gsy_ChOpen() RetVal=0x%x from GSY_GET_PATH_INFO",
							RetVal);
        RespVal = GSY_RSP_ERR_SYS_PATH;
	    gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_CONFIG, LSA_NULL, 0);
    }

	GSY_FUNCTION_TRACE_03(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,"  < gsy_ChOpen() pChSys=%X pChUsr=%X RespVal=0x%x",
					pChSys, pChUsr, RespVal);
    return(RespVal);
}

/*****************************************************************************/
/* Internal function: gsy_CloseChSys()                                       */
/*****************************************************************************/
LSA_UINT16  GSY_LOCAL_FCT_ATTR  gsy_CloseChSys(
GSY_CH_SYS_PTR		pChSys)
{
	LSA_UINT16 PortId;
	LSA_UINT16 RespVal = GSY_RSP_ERR_SEQUENCE;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelchat*/,"  > gsy_CloseChSys() pChSys=%X Handle=%d State=0x%x",
					pChSys, pChSys->Handle, pChSys->State);

	/* If the channel is open and there are no pending requests...
	*/
	if ((pChSys->QueueUpper.Count == 1)
	&&  ((pChSys->State == GSY_CHA_STATE_OPEN)
	 ||  (pChSys->State == GSY_CHA_STATE_CLOSING)))
	{
		if ((pChSys->UserCount == 0)
		&&  LSA_HOST_PTR_ARE_EQUAL(pChSys->pChAnno, LSA_NULL))
		{
			if (pChSys->State == GSY_CHA_STATE_OPEN)
			{
				/* Stop delay measurement at all ports
				*/
				for (PortId = 1; PortId <= pChSys->PortCount; PortId++)
					gsy_DelayUserCtrl(pChSys, PortId, GSY_DELAY_END);
			}

			/* Start closing the lower system channel by cancelling all lower requests
			*/
			pChSys->State = GSY_CHA_STATE_CLOSING;
			RespVal = gsy_CancelLower(pChSys, LSA_TRUE, 0, 0, LSA_NULL, GSY_LOWER_CANCEL);
		}
		else
		{
			GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_CloseChSys() pChSys=%X Handle=%d UserCount=%u: user or anno channel open",
					pChSys, pChSys->Handle, pChSys->UserCount);
		}
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelchat*/,"  < gsy_CloseChSys() pChSys=%X UserCount=%d RespVal=0x%x",
					pChSys, pChSys->UserCount, RespVal);
    return(RespVal);
}

/*****************************************************************************/
/* Internal function: gsy_CancelChAnno()                                     */
/*****************************************************************************/
static LSA_VOID  GSY_LOCAL_FCT_ATTR  gsy_CancelChAnno(
GSY_CH_SYS_PTR		pChSys,
LSA_UINT16			RespVal)
{
	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelchat*/,"  > gsy_CancelChAnno() pChSys=%X pChAnno=%X RespVal=0x%x",
					pChSys, pChSys->pChAnno, RespVal);

	/* Start closing lower announce channel by cancelling all lower requests
	*/
	pChSys->pChAnno->State = GSY_CHA_STATE_CLOSING;
	RespVal = gsy_CancelAnnoAll(pChSys->pChAnno);

	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelchat*/,"  < gsy_CancelChAnno() pChSys=%X RespVal=0x%x",
					pChSys, RespVal);
}

/*****************************************************************************/
/* Internal function: gsy_CloseChAnno()                                      */
/*****************************************************************************/
LSA_UINT16  GSY_LOCAL_FCT_ATTR  gsy_CloseChAnno(
GSY_CH_ANNO_PTR		pChAnno)
{
	LSA_UINT16 RespVal = GSY_RSP_ERR_SEQUENCE;

	GSY_FUNCTION_TRACE_03(pChAnno->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelchat*/,"  > gsy_CloseChAnno() pChAnno=%X Handle=%d State=0x%x",
					pChAnno, pChAnno->Handle, pChAnno->State);

	/* If channel is open and no requests are queued...
	*/
	if ((pChAnno->QueueUpper.Count == 1)
	&&  ((pChAnno->State == GSY_CHA_STATE_OPEN)
	 ||  (pChAnno->State == GSY_CHA_STATE_CLOSING)))
	{
		if (pChAnno->pChSys->UserCount == 0)
		{
			/* Start closing of lower channel by deinitializing of forwarding
			 * if it has been initialized at open channel
			*/
			if ((pChAnno->pChSys->RxTxNanos == 0)
			||  (pChAnno->State == GSY_CHA_STATE_CLOSING))
				RespVal = GSY_RSP_OK;
			else
			{
				pChAnno->State = GSY_CHA_STATE_CLOSE_FWD;
				RespVal = gsy_FwdChannelExit(pChAnno->pChSys, gsy_CancelChAnno);
			}
			if (GSY_RSP_OK == RespVal)
			{
				/* If the ForwardingExit returned synchronous or was not called:
				 * start closing lower channel by CANCEL_ALL
				*/
				pChAnno->State = GSY_CHA_STATE_CLOSING;
				RespVal = gsy_CancelAnnoAll(pChAnno);
			}
		}
		else
		{
			GSY_ERROR_TRACE_03(pChAnno->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_CloseChAnno() pChAnno=%X Handle=%d State=0x%x: user channel open",
					pChAnno, pChAnno->Handle, pChAnno->State);
		}
	}

	GSY_FUNCTION_TRACE_02(pChAnno->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelchat*/,"  < gsy_CloseChAnno() pChAnno=%X RespVal=0x%x",
					pChAnno, RespVal);
    return(RespVal);
}

/*****************************************************************************/
/* Internal access function: gsy_CloseChUsr()                                */
/*****************************************************************************/
LSA_UINT16  GSY_LOCAL_FCT_ATTR  gsy_CloseChUsr(
GSY_CH_USR_PTR			pChUsr,
GSY_UPPER_RQB_PTR_TYPE  pRbClose)
{
	GSY_UPPER_RQB_PTR_TYPE  pRbu;
	LSA_UINT16 RespVal = GSY_RSP_ERR_SEQUENCE;

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelchat*/,"  > gsy_CloseChUsr() pChUsr=%X pRbClose=%X Handle=%d",
					pChUsr, pRbClose, pChUsr->Handle);

	if ((pChUsr->State == GSY_CHA_STATE_OPEN)
    &&  !LSA_HOST_PTR_ARE_EQUAL(pChUsr->pChSys, LSA_NULL)
    &&  !LSA_HOST_PTR_ARE_EQUAL(pChUsr->pChSys->pChAnno, LSA_NULL)
	&&  (pChUsr->pChSys->UserCount != 0)
	&&  (pChUsr->QueueUpper.Count == 0))
	{
		RespVal = GSY_RSP_OK;
		pChUsr->State = GSY_CHA_STATE_CLOSING;

		/* Cancel upper PRM-Indication RQBs
		*/
		while (pChUsr->QueuePrm.Count != 0)
		{
			pRbu = pChUsr->QueuePrm.pFirst;
			gsy_DequeUpper(&pChUsr->QueuePrm, pRbu);
			GSY_UPPER_RQB_PRMIND_ACTCNT_SET(pRbu, 0);
			gsy_CallbackUsr(pChUsr, pRbu, GSY_RSP_OK_CANCEL);
		}
		/* 120207lrg001: Cancel upper PRM-Indication RQBs, which wait for lower Confirmation
		*/
		while (pChUsr->QueueEvent.Count != 0)
		{
			pRbu = pChUsr->QueueEvent.pFirst;
			gsy_DequeUpper(&pChUsr->QueueEvent, pRbu);
			GSY_UPPER_RQB_PRMIND_ACTCNT_SET(pRbu, 0);
			gsy_CallbackUsr(pChUsr, pRbu, GSY_RSP_OK_CANCEL);
		}

		/* Cancel upper Receive RQBs
		*/
		while (pChUsr->QueueRcv.Count != 0)
		{
			pRbu = pChUsr->QueueRcv.pFirst;
			gsy_DequeUpper(&pChUsr->QueueRcv, pRbu);
			gsy_CallbackUsr(pChUsr, pRbu, GSY_RSP_OK_CANCEL);
		}

		/* Cancel upper Diagnose RQBs
		*/
        (LSA_VOID)gsy_DiagCancel(pChUsr, 0);

		if ((GSY_MASTER_STATE_OFF != pChUsr->Master.State)
		||  (GSY_SLAVE_STATE_OFF != pChUsr->Slave.State))
		{
		/* Stop running slave or master
		*/
			pChUsr->MasterControlState = GSY_MASTER_CONTROL_STATE_NONE;
			RespVal = gsy_SyncStop(pChUsr, pRbClose);
		}

		if (GSY_PLL_STATE_SYNC_OFF != pChUsr->PLLData.State)
		{
			/* Stop PLL synchronisation
			*/
			GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_CloseChUsr stopping PLL sync: SlaveStatate=%u MasterState=%u PLLState=%u", 
						pChUsr->Slave.State, pChUsr->Master.State, pChUsr->PLLData.State);

			if (GSY_HW_TYPE_SLAVE_HW == (pChUsr->pChSys->HardwareType & GSY_HW_TYPE_SLAVE_MSK))
			{
				/* Stop PLL synchronisation in KRISC32
				*/
			}
			else
			{
				/* all will be done by gsy_MasterPLLTimer()
				*/
				RespVal = GSY_RSP_OK_ACTIVE;
				pChUsr->PLLData.State = GSY_PLL_STATE_SYNC_STOP;
			}
		}
		if (GSY_RSP_OK == RespVal)
		{
		    /* Put RQB in upper channel queue and close channel
		     */
			gsy_PutUpperQ(&pChUsr->QueueUpper, pRbClose);
			gsy_CloseChUsrDone(pChUsr);
		}
	}

	GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelchat*/,"  < gsy_CloseChUsr() pChUsr=%X RespVal=0x%x",
					pChUsr, RespVal);
    return(RespVal);
}

/*****************************************************************************/
/* Internal function: gsy_CloseChUsrDone()                                   */
/* Finish closing of a user channel                                          */
/*****************************************************************************/
LSA_VOID  GSY_LOCAL_FCT_ATTR  gsy_CloseChUsrDone(
GSY_CH_USR_PTR			pChUsr)
{
	GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelchat*/,"  > gsy_CloseChUsrDone() pChUsr=%X Handle=%d",
					pChUsr, pChUsr->Handle);

	gsy_ChUsrClean(pChUsr);

	/* Callback Close-RQB from upper Queue and free channel structure
	*/
	gsy_CallbackUsr(pChUsr, LSA_NULL, GSY_RSP_OK);
	gsy_ChUsrFree(pChUsr);

	GSY_FUNCTION_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelchat*/,"  < gsy_CloseChUsrDone() pChUsr=%X",
					pChUsr);
}

/*****************************************************************************/
/* Internal function: gsy_CallbackSys()                                      */
/*****************************************************************************/
LSA_VOID  GSY_LOCAL_FCT_ATTR  gsy_CallbackSys(
GSY_CH_SYS_PTR			pChSys,
GSY_UPPER_RQB_PTR_TYPE  pRbu,
LSA_UINT16				RespVal)
{
	LSA_UINT32	TraceIdx = 0;
	GSY_UPPER_CALLBACK_FCT_PTR_TYPE	pCbf = 0;

    if(!LSA_HOST_PTR_ARE_EQUAL(pChSys, LSA_NULL))
    {
		TraceIdx = pChSys->TraceIdx;
		GSY_UPPER_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelnote*/,">>> gsy_CallbackSys() pChSys=%X pRbu=%X RespVal=0x%x",
					pChSys, pRbu, RespVal);

		if (LSA_HOST_PTR_ARE_EQUAL(pRbu, LSA_NULL))
		{
			/* Take first RQB from Upper Queue
			*/
			pRbu = pChSys->QueueUpper.pFirst;
		}
		if (LSA_HOST_PTR_ARE_EQUAL(pRbu, LSA_NULL))
		{
			/* No request block for callback ?!?
			*/
			GSY_ERROR_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** gsy_CallbackSys() pChSys=%X pRbu=NULL RespVal=0x%x",
					pChSys, RespVal);
			gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_NO_RBU, LSA_NULL, 0);
		}
		else
		{
	        /* Remove RQB from Queue and set handle
	        */
	        gsy_DequeUpper(&pChSys->QueueUpper, pRbu);
	        GSY_UPPER_RQB_HANDLE_SET(pRbu, pChSys->HandleUpper);
	    	if (GSY_UPPER_RQB_OPCODE_GET(pRbu) == GSY_OPC_OPEN_CHANNEL)
			{
				/* OPEN CHANNEL: return own Handle and set Channel
				 *               State to OPEN if response is OK 
				*/
		        GSY_UPPER_RQB_HANDLE_LOWER_SET(pRbu, pChSys->Handle);
				if (RespVal == GSY_RSP_OK)
					pChSys->State = GSY_CHA_STATE_OPEN;

			}
				
			/* Use Upper callback function stored in channel structure
			*/
	        pCbf = pChSys->pCbf;
		}
    }
    else 
	{
		/* No channel! Error at OPEN CHANNEL?
		*/
		GSY_UPPER_TRACE_02(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_FATAL,">>> gsy_CallbackSys() pChSys=NULL pRbu=%X RespVal=0x%x",
					pRbu, RespVal);
		gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
	}

    if ((!(0 == pCbf)         )            /* function is called too for system services */
	&&	(!LSA_HOST_PTR_ARE_EQUAL(pRbu, LSA_NULL)))       //lint !e774 Boolean within 'if' always evaluates to True, it depends on the global lint rule -function( exit, gsy_ErrorInternal), HM 20.05.2016
    {
        /* Set Responsecode and pass RQB to Callbackfunktion of upper Layer
		*/
#ifdef GSY_MESSAGE
		/* Todo: Error structure
		*/
#endif /* GSY_MESSAGE */
		GSY_UPPER_RQB_RESPONSE_SET(pRbu, RespVal);
		GSY_UPPER_RQB_ERR_COMPID_SET(pRbu, LSA_COMP_ID_UNUSED);

 		GSY_UPPER_TRACE_04(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelnote*/,"< < gsy_CallbackSys(Opcode=%d) pRbu=%X Response=0x%x HandleUpper=%d", 
						GSY_UPPER_RQB_OPCODE_GET(pRbu), pRbu, GSY_UPPER_RQB_RESPONSE_GET(pRbu), GSY_UPPER_RQB_HANDLE_GET(pRbu));
//		pCbf(pRbu);
		GSY_REQUEST_UPPER_DONE(pCbf, pRbu, pChSys->pSys);
	}
	else 
	{
		GSY_ERROR_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** gsy_CallbackSys() pCbf=%X pRbu=%X", pCbf, pRbu);
		gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
	}

	GSY_UPPER_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelnote*/,"<<< gsy_CallbackSys() pRbu=%X pCbf=%X",
					pRbu, pCbf);
}

/*****************************************************************************/
/* Internal function: gsy_CallbackAnno()                                     */
/*****************************************************************************/
LSA_VOID  GSY_LOCAL_FCT_ATTR  gsy_CallbackAnno(
GSY_CH_SYS_PTR			pChSys,
GSY_UPPER_RQB_PTR_TYPE  pRbu,
LSA_UINT16				RespVal)
{
	LSA_UINT32	TraceIdx = 0;
	GSY_UPPER_CALLBACK_FCT_PTR_TYPE	pCbf = 0;

    if(!LSA_HOST_PTR_ARE_EQUAL(pChSys, LSA_NULL))
    {
		TraceIdx = pChSys->TraceIdx;
		GSY_UPPER_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelnote*/,">>> gsy_CallbackAnno() pChSys=%X pRbu=%X RespVal=0x%x",
					pChSys, pRbu, RespVal);

		if (LSA_HOST_PTR_ARE_EQUAL(pRbu, LSA_NULL))
		{
			/* Take first requst block from upper queue
			*/
			pRbu = pChSys->pChAnno->QueueUpper.pFirst;
		}
		if (LSA_HOST_PTR_ARE_EQUAL(pRbu, LSA_NULL))
		{
			/* No upper RQB ?!?
			*/
			gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_NO_RBU, LSA_NULL, 0);
		}
		else
		{
	        /* Remove RQB from queue and set handle
	        */
	        gsy_DequeUpper(&pChSys->pChAnno->QueueUpper, pRbu);
	        GSY_UPPER_RQB_HANDLE_SET(pRbu, pChSys->pChAnno->HandleUpper);
	    	if (GSY_UPPER_RQB_OPCODE_GET(pRbu) == GSY_OPC_OPEN_CHANNEL)
			{
				/* OPEN CHANNEL: return own handle and set channel state to OPEN if OK 
				*/
		        GSY_UPPER_RQB_HANDLE_LOWER_SET(pRbu, pChSys->pChAnno->Handle);
				if (RespVal == GSY_RSP_OK)
					pChSys->pChAnno->State = GSY_CHA_STATE_OPEN;

			}
				
			/* Use upper callback function from channel data
			*/
	        pCbf = pChSys->pChAnno->pCbf;
		}
	}
    else 
	{
		/* No channel! Error at OPEN CHANNEL?
		*/
		GSY_UPPER_TRACE_02(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_FATAL,">>> gsy_CallbackAnno() pChSys=NULL pRbu=%X RespVal=0x%x",
					pRbu, RespVal);
		gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
	}

    if ((!(0 == pCbf)                                  )            /* function is called too for system services */
	&&	(!LSA_HOST_PTR_ARE_EQUAL(pRbu, LSA_NULL       )))       //lint !e774 Boolean within 'if' always evaluates to True, it depends on the global lint rule -function( exit, gsy_ErrorInternal), HM 20.05.2016
    {
        /* Set response code and call callback function of upper layer
		*/
#ifdef GSY_MESSAGE
		/* Todo: Fill error structure of RQB
		*/
#endif /* GSY_MESSAGE */
		GSY_UPPER_RQB_RESPONSE_SET(pRbu, RespVal);
		GSY_UPPER_RQB_ERR_COMPID_SET(pRbu, LSA_COMP_ID_UNUSED);

 		GSY_UPPER_TRACE_04(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelnote*/,"< < gsy_CallbackAnno(Opcode=%d) pRbu=%X Response=0x%x HandleUpper=%d", 
						GSY_UPPER_RQB_OPCODE_GET(pRbu), pRbu, GSY_UPPER_RQB_RESPONSE_GET(pRbu), GSY_UPPER_RQB_HANDLE_GET(pRbu));

		GSY_REQUEST_UPPER_DONE(pCbf, pRbu, pChSys->pChAnno->pSys);
	}
	else 
	{
		GSY_ERROR_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** gsy_CallbackAnno() pCbf=%X pRbu=%X", pCbf, pRbu);
		gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
	}

	GSY_UPPER_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelnote*/,"<<< gsy_CallbackAnno() pRbu=%X pCbf=%X",
					pRbu, pCbf);
}

/*****************************************************************************/
/* Internal function: gsy_CallbackUsr()                                      */
/*****************************************************************************/
LSA_VOID  GSY_LOCAL_FCT_ATTR  gsy_CallbackUsr(
GSY_CH_USR_PTR			pChUsr,
GSY_UPPER_RQB_PTR_TYPE  pRbu,
LSA_UINT16				RespVal)
{
	LSA_UINT32	TraceIdx = 0;
	GSY_UPPER_CALLBACK_FCT_PTR_TYPE	pCbf = 0;

    if(!LSA_HOST_PTR_ARE_EQUAL(pChUsr, LSA_NULL))
    {
		TraceIdx = pChUsr->TraceIdx;
		GSY_UPPER_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelnote*/, ">>> gsy_CallbackUsr() pChUsr=%X pRbu=%X RespVal=0x%x",
				pChUsr, pRbu, RespVal);

		if (LSA_HOST_PTR_ARE_EQUAL(pRbu, LSA_NULL))
		{
			/* Take first RQB from Upper Queue
			*/
			pRbu = pChUsr->QueueUpper.pFirst;
		}
		if (LSA_HOST_PTR_ARE_EQUAL(pRbu, LSA_NULL))
		{
			/* No request block for callback ?!?
			*/
			gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_NO_RBU, LSA_NULL, 0);
		}
		else
		{
	        /* Remove RQB from Queue and set handle
	        */
	        gsy_DequeUpper(&pChUsr->QueueUpper, pRbu);
	        GSY_UPPER_RQB_HANDLE_SET(pRbu, pChUsr->HandleUpper);
	    	if (GSY_UPPER_RQB_OPCODE_GET(pRbu) == GSY_OPC_OPEN_CHANNEL)
			{
				/* OPEN CHANNEL: return own Handle and set Channel
				 *               State to OPEN if response is OK 
				*/
		        GSY_UPPER_RQB_HANDLE_LOWER_SET(pRbu, pChUsr->Handle);
				if (RespVal == GSY_RSP_OK)
					pChUsr->State = GSY_CHA_STATE_OPEN;

			}
				
			/* Use Upper callback function stored in channel structure
			*/
	        pCbf = pChUsr->pCbf;
		}
    }
    else 
	{
		/* No channel! Error at OPEN CHANNEL?
		*/
		GSY_UPPER_TRACE_02(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_FATAL, ">>> gsy_CallbackUsr() pChUsr=NULL pRbu=%X RespVal=0x%x",
				pRbu, RespVal);
		gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
	}

    if ((!(0 == pCbf)                           )            /* function is called too for system services */
	&&	(!LSA_HOST_PTR_ARE_EQUAL(pRbu, LSA_NULL)))      //lint !e774 Boolean within 'if' always evaluates to True, it depends on the global lint rule -function( exit, gsy_ErrorInternal), HM 20.05.2016
    {
        /* Set Responsecode and pass RQB to Callbackfunktion of upper Layer
		*/
#ifdef GSY_MESSAGE
		/* Todo: Error structure
		*/
#endif /* GSY_MESSAGE */
		GSY_UPPER_RQB_RESPONSE_SET(pRbu, RespVal);
		GSY_UPPER_RQB_ERR_COMPID_SET(pRbu, LSA_COMP_ID_UNUSED);

 		GSY_UPPER_TRACE_04(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelnote*/,"< < gsy_CallbackUsr(Opcode=%d) pRbu=%X Response=0x%x HandleUpper=%d", 
						GSY_UPPER_RQB_OPCODE_GET(pRbu), pRbu, GSY_UPPER_RQB_RESPONSE_GET(pRbu), GSY_UPPER_RQB_HANDLE_GET(pRbu));

		GSY_REQUEST_UPPER_DONE(pCbf, pRbu, pChUsr->pSys);
	}
	else 
	{
		GSY_ERROR_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** gsy_CallbackUsr() pCbf=%X pRbu=%X", pCbf, pRbu);
		gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
	}

	GSY_UPPER_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelnote*/,"<<< gsy_CallbackUsr() pRbu=%X pCbf=%X",
					pRbu, pCbf);
}

/*****************************************************************************/
/* Internal function: gsy_CallbackRbu()                                      */
/*****************************************************************************/
LSA_VOID  GSY_LOCAL_FCT_ATTR  gsy_CallbackRbu(
LSA_SYS_PTR_TYPE		pSys,
GSY_UPPER_RQB_PTR_TYPE  pRbu,
LSA_UINT16				RespVal)
{
	GSY_UPPER_CALLBACK_FCT_PTR_TYPE	pCbf = 0;

	GSY_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,">>> gsy_CallbackRbu() pRbu=%X RespVal=0x%x",
					pRbu, RespVal);

	if (LSA_HOST_PTR_ARE_EQUAL(pRbu, LSA_NULL))
	{
		/* No request block for callback ?!?
		*/
		GSY_ERROR_TRACE_00(0, LSA_TRACE_LEVEL_FATAL,"*** gsy_CallbackRbu() pRbu=NULL");
		gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_NO_RBU, LSA_NULL, 0);
	}
	else
	{
    	if (GSY_UPPER_RQB_OPCODE_GET(pRbu) == GSY_OPC_OPEN_CHANNEL)
		{
			/* Get Upper callback function and upper Handle from RQB 
			*/
			GSY_UPPER_RQB_HANDLE_SET(pRbu, GSY_UPPER_RQB_HANDLE_UPPER_GET(pRbu));
			pCbf = GSY_UPPER_RQB_CBF_GET(pRbu);
		}
	    else
		{
			GSY_ERROR_TRACE_03(0, LSA_TRACE_LEVEL_FATAL,"*** gsy_CallbackRbu() pCBF=%X,pRbu=%X: invalid opcode=%u",
					pCbf, pRbu, GSY_UPPER_RQB_OPCODE_GET(pRbu));
	    	gsy_ErrorUser(GSY_MODULE_ID, __LINE__, GSY_ERR_BAD_HANDLE, pRbu, sizeof(GSY_RQB_TYPE));
		}
	}

    if ((!(0 == pCbf)                                  )            /* function is called too for system services */
	&&	(!LSA_HOST_PTR_ARE_EQUAL(pRbu, LSA_NULL       )))      //lint !e774 Boolean within 'if' always evaluates to True, it depends on the global lint rule -function( exit, gsy_ErrorInternal), HM 20.05.2016
    {
        /* Set Responsecode and pass RQB to Callbackfunktion of upper Layer
		*/
#ifdef GSY_MESSAGE
		/* Todo: Error structure
		*/
#endif /* GSY_MESSAGE */
		GSY_UPPER_RQB_RESPONSE_SET(pRbu, RespVal);
		GSY_UPPER_RQB_ERR_COMPID_SET(pRbu, LSA_COMP_ID_UNUSED);

 		GSY_UPPER_TRACE_04(0, LSA_TRACE_LEVEL_NOTE,"< < gsy_CallbackRbu(Opcode=%d) pRbu=%X Response=0x%x HandleUpper=%d", 
						GSY_UPPER_RQB_OPCODE_GET(pRbu), pRbu, GSY_UPPER_RQB_RESPONSE_GET(pRbu), GSY_UPPER_RQB_HANDLE_GET(pRbu));

		GSY_REQUEST_UPPER_DONE(pCbf, pRbu, pSys);
	}
	else 
	{
		GSY_ERROR_TRACE_02(0, LSA_TRACE_LEVEL_FATAL,"*** gsy_CallbackRbu() pCbf=%X pRbu=%X", pCbf, pRbu);
		gsy_ErrorUser(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
	}

	GSY_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,"<<< gsy_CallbackRbu() pRbu=%X pCbf=%X",
					pRbu, pCbf);
}

/*****************************************************************************/
/* External LSA access function: gsy_open_channel()                          */
/*****************************************************************************/
LSA_VOID  GSY_UPPER_IN_FCT_ATTR  gsy_open_channel(
GSY_UPPER_RQB_PTR_TYPE  pRbu)
{
	LSA_UINT16  RespVal = GSY_RSP_OK_ACTIVE;
    LSA_OPCODE_TYPE Opcode;                                   \

    GSY_ENTER();
	GSY_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,">>> gsy_open_channel() pRbu=%X",
					pRbu);

    if(!LSA_HOST_PTR_ARE_EQUAL(pRbu, LSA_NULL))
    {
		/* Mark RQB active
		*/
		GSY_UPPER_RQB_RESPONSE_SET(pRbu, GSY_RSP_OK_ACTIVE);
		Opcode = GSY_UPPER_RQB_OPCODE_GET(pRbu);
		switch(Opcode)
		{
		case GSY_OPC_OPEN_CHANNEL:
			if(0 != GSY_UPPER_RQB_CBF_GET(pRbu))
			{
				RespVal = gsy_ChOpen(pRbu);
			}
			else
			{
				RespVal = GSY_RSP_ERR_PARAM;
				GSY_ERROR_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"*** gsy_open_channel() pRbu=%X: gsy_request_upper_done_ptr=0", pRbu);
				GSY_UPPER_RQB_RESPONSE_SET(pRbu, RespVal);
				GSY_RQB_ERROR(pRbu);
			}
			break;

		default:
			RespVal = GSY_RSP_ERR_SYNTAX;
			GSY_ERROR_TRACE_02(0, LSA_TRACE_LEVEL_ERROR,"*** gsy_open_channel() pRbu=%X: invalid opcode=%u", pRbu, Opcode);
			GSY_UPPER_RQB_RESPONSE_SET(pRbu, RespVal);
			GSY_RQB_ERROR(pRbu);
		}

		/* On error the RQB is returned without a channel structure
		 * 210710lrg001: now in	gsy_ChOpen() prior to RELEASE_PATH_INFO()

		if ((RespVal != GSY_RSP_OK_ACTIVE)
		&&  (RespVal != GSY_RSP_OK))
		{
			gsy_CallbackSys(LSA_NULL, pRbu, RespVal);
		}
		*/
	}
	else
	{
		GSY_ERROR_TRACE_00(0, LSA_TRACE_LEVEL_FATAL,"*** gsy_open_channel() pRbu=LSA_NULL");
		gsy_ErrorUser(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
	}

	GSY_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"<<< gsy_open_channel() pRbu=%X RespVal=0x%x",
					pRbu, RespVal);
    GSY_EXIT();
    return;
}

/*****************************************************************************/
/* External LSA access function: gsy_close_channel()                         */
/*****************************************************************************/
LSA_VOID  GSY_UPPER_IN_FCT_ATTR  gsy_close_channel(
GSY_UPPER_RQB_PTR_TYPE  pRbu)
{
	LSA_HANDLE_TYPE	Handle;
	LSA_OPCODE_TYPE	Opcode;
	LSA_UINT16		RespVal;
	GSY_CH_SYS_PTR	pChSys;
	GSY_CH_ANNO_PTR	pChAnno = LSA_NULL;
	GSY_CH_USR_PTR	pChUsr = LSA_NULL;
	LSA_UINT32   	TraceIdx = 0;

    GSY_ENTER();
	Handle = GSY_UPPER_RQB_HANDLE_GET(pRbu);
    Opcode = GSY_UPPER_RQB_OPCODE_GET(pRbu);

	GSY_UPPER_TRACE_03(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_NOTE_HIGH,">>> gsy_close_channel() pRbu=%X Opcode=%u Handle=%d",
					pRbu, LSA_NULL, Handle);

    GSY_UPPER_RQB_RESPONSE_SET(pRbu, GSY_RSP_OK_ACTIVE);
    RespVal = GSY_RSP_ERR_SYNTAX;

	pChSys = gsy_ChSysGet(Handle);

    if(!LSA_HOST_PTR_ARE_EQUAL(pChSys, LSA_NULL))
    {
		TraceIdx = pChSys->TraceIdx;
		GSY_UPPER_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"  > gsy_close_channel() pChSys=%X", pChSys);

	    /* System channel: Put RQB into upper channel queue
	     */
		gsy_PutUpperQ(&pChSys->QueueUpper, pRbu);

        if (GSY_OPC_CLOSE_CHANNEL == Opcode)
		{
            RespVal = gsy_CloseChSys(pChSys);
		}

        /* Return RQB here on error
         * else at lower close_channel Confirmation
         */
        if ((RespVal != GSY_RSP_OK_ACTIVE)
		&&  (RespVal != GSY_RSP_OK))
            gsy_CallbackSys(pChSys, pRbu, RespVal);
    }
    else
    {
		/* Try announce channel ...
		*/
		pChAnno = gsy_ChAnnoGet(Handle);

		if (!LSA_HOST_PTR_ARE_EQUAL(pChAnno, LSA_NULL))
		{
			TraceIdx = pChAnno->TraceIdx;
			if (!LSA_HOST_PTR_ARE_EQUAL(pChAnno->pChSys, LSA_NULL))
			{
				GSY_UPPER_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"  > gsy_close_channel() pChAnno=%X", pChAnno);
		
			    /* Put RQB into upper channel queue
			     */
				gsy_PutUpperQ(&pChAnno->QueueUpper, pRbu);

		        if (GSY_OPC_CLOSE_CHANNEL == Opcode)
				{
		            RespVal = gsy_CloseChAnno(pChAnno);
				}

		        /* Return RQB here on error
		         * else at lower close_channel Confirmation
		         */
		        if ((RespVal != GSY_RSP_OK_ACTIVE)
				&&  (RespVal != GSY_RSP_OK))
		            gsy_CallbackAnno(pChAnno->pChSys, pRbu, RespVal);
			}
			else
			{
				GSY_ERROR_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** gsy_close_channel() pChAnno=%X Handle=%d State=0x%x: no system channel",
								pChAnno, pChAnno->Handle, pChAnno->State);
			    gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_CH_PTR, pChAnno, 0);
			}
		}
		else
		{
			/* Try user channel ...
			*/
			pChUsr = gsy_ChUsrGet(Handle);

			if (!LSA_HOST_PTR_ARE_EQUAL(pChUsr, LSA_NULL))
			{
				TraceIdx = pChUsr->TraceIdx;
				GSY_UPPER_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"  > gsy_close_channel() pChUsr=%X", pChUsr);

		        if (GSY_OPC_CLOSE_CHANNEL == Opcode)
				{
		            RespVal = gsy_CloseChUsr(pChUsr, pRbu);
				}

		        /* Return RQB here on error
		        */
		        if ((RespVal != GSY_RSP_OK_ACTIVE)
				&&  (RespVal != GSY_RSP_OK))
		            gsy_CallbackUsr(pChUsr, pRbu, RespVal);
		    }
		    else
		    {
		        RespVal = GSY_RSP_ERR_REF;
		        GSY_UPPER_RQB_RESPONSE_SET(pRbu, RespVal);
		        GSY_RQB_ERROR(pRbu);
		    }
		}
    }

	GSY_UPPER_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"<<< gsy_close_channel() pChSys=%X pChUsr=%X RespVal=0x%x",
					pChSys, pChUsr, RespVal);
    GSY_EXIT();
    return;
}

/*****************************************************************************/
/* External LSA access function: gsy_request()                               */
/*****************************************************************************/
LSA_VOID  GSY_UPPER_IN_FCT_ATTR  gsy_request(
GSY_UPPER_RQB_PTR_TYPE  pRbu)
{            
	LSA_OPCODE_TYPE	Opcode;
	GSY_CH_USR_PTR  pChUsr = LSA_NULL;
	GSY_CH_SYS_PTR  pChSys = LSA_NULL;
	LSA_UINT16		RespVal = GSY_RSP_OK_ACTIVE;
	LSA_UINT32   	TraceIdx = 0;

    GSY_ENTER();
	Opcode = GSY_UPPER_RQB_OPCODE_GET(pRbu);

	GSY_UPPER_TRACE_02(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_NOTE_LOW,">>> gsy_request() pRbu=%X Opcode=%d",
					pRbu, Opcode);

#ifdef GSY_DO_TIMEOUT_EXTERN
    if (GSY_OPC_TIMER == Opcode)
        gsy_TimeoutRequest(pRbu);
	else
#endif
	{
	    /* Set RQB to active
	    */
	    GSY_UPPER_RQB_RESPONSE_SET(pRbu, GSY_RSP_OK_ACTIVE);

	    /* Check Handle and get pointer to user channel structure
	     */
		pChUsr = gsy_ChUsrGet(GSY_UPPER_RQB_HANDLE_GET(pRbu));

	    if(!LSA_HOST_PTR_ARE_EQUAL(pChUsr, LSA_NULL))
	    {
			/* Request on user channel
			*/
			TraceIdx = pChUsr->TraceIdx;
			GSY_UPPER_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"  > gsy_request() pChUsr=%X", pChUsr);

	        if (pChUsr->State != GSY_CHA_STATE_OPEN)
	        {
	            RespVal = GSY_RSP_ERR_SEQUENCE;
				GSY_ERROR_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_request() pChUsr=%X channel state=0x%x not OPEN", pChUsr, pChUsr->State);
	        }
			else if ((pChUsr->pChSys->RxTxNanos == 0)
				 &&  (Opcode != GSY_OPC_PRM_PREPARE)
				 &&  (Opcode != GSY_OPC_PRM_WRITE)
				 &&  (Opcode != GSY_OPC_PRM_READ)
				 &&  (Opcode != GSY_OPC_PRM_END)
				 &&  (Opcode != GSY_OPC_PRM_COMMIT)
				 &&  (Opcode != GSY_OPC_PRM_CHANGE_PORT)
				 &&  (Opcode != GSY_OPC_PRM_INDICATION_PROVIDE)
				 &&  (Opcode != GSY_OPC_DIAG_INDICATION_PROVIDE))
//				 &&  (Opcode != GSY_OPC_CHANNEL_INFO_GET))	//Task 1311090
	        {
				/* Only the above services are allowed on EDD without timestamps
				*/
				RespVal = GSY_RSP_ERR_PROTOCOL;
				GSY_ERROR_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_request() pChUsr=%X Opcode=%u not supported w/o timestamps", pChUsr, Opcode);
	        }
	        else
	        {
			    switch(Opcode)
		        {
					/* Read Delay
					*/
					case GSY_OPC_GET_DELAY:
			        	RespVal = gsy_UserDelayGet(pChUsr->pChSys, pRbu);
						break;

					/* Prepare record loading 
					*/
				    case GSY_OPC_PRM_PREPARE:
			        	RespVal = gsy_PrmPrepare(pChUsr, pRbu);
						break;

					/* Load record
					*/
				    case GSY_OPC_PRM_WRITE:
						/* 131206lrg001: also on EDD without timestamps
						*/
			        	RespVal = gsy_PrmWrite(pChUsr, pRbu);
						break;

				    case GSY_OPC_PRM_CHANGE_PORT:
			        	RespVal = gsy_PrmChangePort(pChUsr, pRbu);
						break;

					/* Finish loading records
					*/
				    case GSY_OPC_PRM_END:
			        	RespVal = gsy_PrmEnd(pChUsr, pRbu);
						break;

					/* Activate loaded records = (Re)start of Master/Slave
					*/
				    case GSY_OPC_PRM_COMMIT:
			        	RespVal = gsy_PrmCommit(pChUsr, pRbu);
						break;

					/* Read active Record or Diagnosis record
					*/
				    case GSY_OPC_PRM_READ:
						RespVal = GSY_RSP_ERR_PRM_INDEX;
						if (pChUsr->pChSys->RxTxNanos != 0)
				        	RespVal = gsy_PrmRead(pChUsr, pRbu);
						break;

					/* PRM-Event Indication provide
					 * 
					*/
				    case GSY_OPC_PRM_INDICATION_PROVIDE:
						RespVal = gsy_PrmUserInd(pChUsr, pRbu);
						if (RespVal == GSY_RSP_OK_ACTIVE)
						{
					        gsy_PutUpperQ(&pChUsr->QueuePrm, pRbu);
						}
						/* 230807lrg001: the acual RQB must not get ahead the RQBs waiting  
						 *               for the lower confimation in the QueueEvent
						*/
						else if (pChUsr->QueueEvent.Count != 0)
						{
					        gsy_PutUpperQ(&pChUsr->QueueEvent, pRbu);
							RespVal = GSY_RSP_OK_ACTIVE;
						}
						break;

					/* Diagnose-Event Indication provide
					 * 
					*/
				    case GSY_OPC_DIAG_INDICATION_PROVIDE:
						RespVal = gsy_DiagUserGet(pChUsr->pChSys, pRbu);
						if (GSY_RSP_OK_ACTIVE == RespVal)
				        	gsy_PutUpperQ(&pChUsr->pChSys->QueueDiag[GSY_UPPER_RQB_DIAGIND_SYNCID_GET(pRbu)], pRbu);
						else
							GSY_UPPER_RQB_DIAGIND_INDCNT_SET(pRbu, pChUsr->pChSys->QueueDiag[pChUsr->SyncId].Count);

						break;

					/* Data receive: check parameters (length has to be >= size of the FrameId)
					*/
				    case GSY_OPC_RECV_DATA:
						RespVal = GSY_RSP_OK_ACTIVE;
						if (LSA_HOST_PTR_ARE_EQUAL(GSY_UPPER_RQB_RECV_DATA_PTR_GET(pRbu), LSA_NULL)
						||  (GSY_UPPER_RQB_RECV_DATA_LEN_GET(pRbu) < 2))
							RespVal = GSY_RSP_ERR_PARAM;
						else
					        gsy_PutUpperQ(&pChUsr->QueueRcv, pRbu);
						break;

					/* GetParams: read configured values and statistics
					*/
				    case GSY_OPC_SYNC_GET_PARAMS:
						RespVal = gsy_SyncParams(pChUsr, pRbu);
						break;

					/* Cancel Data receive and/or Sync-Event Indication RQBs
					*/
				    case GSY_OPC_CANCEL_IND:
						RespVal = gsy_CancelUpper(pChUsr, pRbu);
						break;

					/* Read channel information (SyncId)
				    case GSY_OPC_CHANNEL_INFO_GET:	//Task 1311090
			        	GSY_UPPER_RQB_CHANNELINFO_SYNCID_SET(pRbu, pChUsr->SyncId);
						RespVal = GSY_RSP_OK;
						break;
					*/

					/* Requests the start or stop of the PTCP time master
					*/
				    case GSY_OPC_MASTER_CONTROL:
						RespVal = gsy_MasterControl(pChUsr, pRbu);
						break;

			        default:
			            RespVal = GSY_RSP_ERR_SYNTAX;
		        }

	        }

	        /* Callback the RQB of a finished request
	         */
	        if(RespVal != GSY_RSP_OK_ACTIVE)
				gsy_CallbackUsr(pChUsr, pRbu, RespVal);
	    }
	    else
	    {
			/* Check Handle and get system channel pointer
			 */
			pChSys = gsy_ChSysGet(GSY_UPPER_RQB_HANDLE_GET(pRbu));

			if(!LSA_HOST_PTR_ARE_EQUAL(pChSys, LSA_NULL))
			{
				/* Request on system channel
				*/
				TraceIdx = pChSys->TraceIdx;
				GSY_UPPER_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"  > gsy_request() pChSys=%X", pChSys);

		        gsy_PutUpperQ(&pChSys->QueueUpper, pRbu);
			    if (pChSys->State != GSY_CHA_STATE_OPEN)
			    {
			        RespVal = GSY_RSP_ERR_SEQUENCE;
					GSY_ERROR_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_request() pChSys=%X channel state=0x%x not OPEN", pChSys, pChSys->State);
			    }
				else if (pChSys->RxTxNanos == 0)
		        {
					/* EDD without timestamps
					*/
					RespVal = GSY_RSP_ERR_PROTOCOL;
					GSY_ERROR_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_request() pChUsr=%X request not supported w/o timestamps", pChSys);
		        }
			    else 
			    {
					switch(Opcode)
					{
						/* Read Delay
						*/
						case GSY_OPC_GET_DELAY:
				        	RespVal = gsy_UserDelayGet(pChSys, pRbu);
							break;

						default:
							RespVal = GSY_RSP_ERR_SYNTAX;
					}
			    }
		        /* Callback the RQB of a finished request
		        */
		        if(RespVal != GSY_RSP_OK_ACTIVE)
					gsy_CallbackSys(pChSys, pRbu, RespVal);
			}
			else
			{
		        /* Invalid Handle
				*/
		        RespVal = GSY_RSP_ERR_REF;
		        GSY_UPPER_RQB_RESPONSE_SET(pRbu, RespVal);
		        GSY_RQB_ERROR(pRbu);
			}
	    }
	}

	GSY_UPPER_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"<<< gsy_request() pChSys=%X pChUsr=%X RespVal=0x%x",
					pChSys, pChUsr, RespVal);
    GSY_EXIT();
    return;
}

/*****************************************************************************/
/* Internal function: gsy_CancelUpper()                                      */
/* Cancel Receive Data and/or Sync Indication RQBs                           */
/*****************************************************************************/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_CancelUpper(
GSY_CH_USR_PTR			pChUsr,
GSY_UPPER_RQB_PTR_TYPE  pRbu)
{
	LSA_OPCODE_TYPE	Opcode = GSY_UPPER_RQB_CANCEL_IND_OPCODE_GET(pRbu);
	LSA_UINT16		Count = GSY_UPPER_RQB_CANCEL_IND_COUNT_GET(pRbu);
	LSA_UINT16		UpCount = 0;
	LSA_UINT16		RespVal = GSY_RSP_OK;
	GSY_UPPER_RQB_PTR_TYPE  pRb = LSA_NULL;

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_CancelUpper() pChUsr=%X Opcode=%d Count=%d",
					pChUsr, Opcode, Count);

	if ((GSY_OPC_PRM_INDICATION_PROVIDE == Opcode)
//	||  (GSY_OPC_SYNC_INDICATION_PROVIDE == Opcode)
	||  (GSY_OPC_DIAG_INDICATION_PROVIDE == Opcode)
	||  (GSY_OPC_RECV_DATA == Opcode)
	||  (GSY_OPC_ALL == Opcode))
	{
		if (((GSY_OPC_PRM_INDICATION_PROVIDE == Opcode) || (GSY_OPC_ALL == Opcode))
		&&  (pChUsr->QueuePrm.Count != 0))
		{
			/* Cancel PRM-Indication RQBs 
			*/
			pRb = pChUsr->QueuePrm.pFirst;
			while (!LSA_HOST_PTR_ARE_EQUAL(pRb, LSA_NULL)
			&&     ((0 == Count) || (UpCount < Count)))
			{
				gsy_DequeUpper(&pChUsr->QueuePrm, pRb);
				GSY_UPPER_RQB_PRMIND_ACTCNT_SET(pRb, 0);
				gsy_CallbackUsr(pChUsr, pRb, GSY_RSP_OK_CANCEL);
				UpCount++;
				pRb = pChUsr->QueuePrm.pFirst;
			}
		}
		if (((GSY_OPC_RECV_DATA == Opcode) || (GSY_OPC_ALL == Opcode))
		&&  (pChUsr->QueueRcv.Count != 0))
		{
			/* While an upper RQB is in the Indication Queue
			 * and UserCount is not reached:
			 * Return RQB to user with  Response=GSY_RSP_OK_CANCEL
			*/
			pRb = pChUsr->QueueRcv.pFirst;
			while (!LSA_HOST_PTR_ARE_EQUAL(pRb, LSA_NULL)
			&&     ((0 == Count) || (UpCount < Count)))
			{
		        gsy_DequeUpper(&pChUsr->QueueRcv, pRb);
				GSY_UPPER_RQB_RECV_DATA_LEN_SET(pRb, 0);
				gsy_CallbackUsr(pChUsr, pRb, GSY_RSP_OK_CANCEL);
				UpCount++;
				pRb = pChUsr->QueueRcv.pFirst;
			}
		}
		if (((GSY_OPC_DIAG_INDICATION_PROVIDE == Opcode) || (GSY_OPC_ALL == Opcode))
		&&  ((0 == Count) || (UpCount < Count)))
		{
			/* Cancel diagnosis indication RQBs
			*/
			if	(0 == Count)
		        UpCount += gsy_DiagCancel(pChUsr, Count);
			else
		        UpCount += gsy_DiagCancel(pChUsr, (LSA_UINT16)(Count - UpCount));
		}
	}
	else
		RespVal = GSY_RSP_ERR_PARAM;

	/* Return count of cancelled RQBs
	*/
	GSY_UPPER_RQB_CANCEL_IND_COUNT_SET(pRbu, UpCount);

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_CancelUpper() pRbu=%X UpCount=%d RespVal=0x%x",
					pRbu, UpCount, RespVal);
	return(RespVal);
}

/*****************************************************************************/
/* Internal function: gsy_DiagCancel()                                       */
/* Cancel Diagnose Indication RQBs                                           */
/*****************************************************************************/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_DiagCancel(
GSY_CH_USR_PTR			pChUsr,
LSA_UINT16  			Count)
{
	LSA_UINT8				SyncId;
	LSA_UINT16				UpCount = 0;
	GSY_UPPER_RQB_PTR_TYPE  pRbu = LSA_NULL;

	GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_DiagCancel() pChUsr=%X Count=%d",
					pChUsr, Count);

	for (SyncId = 0; SyncId < GSY_MAX_PROTOCOLS; SyncId++)
	{
		/* While an upper RQB is in the Indication Queue
		 * and UserCount is not reached:
		*/
		pRbu = pChUsr->pChSys->QueueDiag[SyncId].pFirst;
		while (!LSA_HOST_PTR_ARE_EQUAL(pRbu, LSA_NULL)
		&&     ((0 == Count) || (UpCount < Count)))
		{
			if (GSY_UPPER_RQB_HANDLE_GET(pRbu) == pChUsr->Handle)
			{
				/* If the upper RQB in the system channel queue if from this user channel:
				 * Return RQB to user with  Response=GSY_RSP_OK_CANCEL
				*/
				gsy_DequeUpper(&pChUsr->pChSys->QueueDiag[SyncId], pRbu);
				gsy_CallbackUsr(pChUsr, pRbu, GSY_RSP_OK_CANCEL);
				UpCount++;
				pRbu = pChUsr->pChSys->QueueDiag[SyncId].pFirst;
			}
			else
				pRbu = GSY_UPPER_RQB_NEXT_GET(pRbu);
		}
	}

	GSY_FUNCTION_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_DiagCancel() UpCount=0x%x",
					UpCount);

	/* Return count of cancelled RQBs
	*/
	return(UpCount);
}

/*****************************************************************************/
/* Internal function: gsy_DiagUserGet()                                      */
/* Write actual diagnosis event to user RQB buffer.                          */
/*****************************************************************************/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_DiagUserGet(
GSY_CH_SYS_PTR			pChSys,
GSY_UPPER_RQB_PTR_TYPE  pRbu)
{
	GSY_DRIFT_PTR		pDrift = LSA_NULL;
	LSA_UINT16	 		Trigger = GSY_UPPER_RQB_DIAGIND_TRIGGER_GET(pRbu);
	LSA_UINT16	 		RespVal = GSY_RSP_ERR_PARAM;
	LSA_UINT8			SyncId = GSY_UPPER_RQB_DIAGIND_SYNCID_GET(pRbu);

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_DiagUserGet() pChSys=%X pRbu=%X SyncId=0x%02x",
					pChSys, pRbu, SyncId);

//	if ( (GSY_SYNCID_CLOCK != SyncId) && (GSY_SYNCID_TIME != SyncId) )
	if (0 == (((LSA_UINT32)(SyncId + 1)) & GSY_SYNC_ID_SUPPORTED))
	{
		GSY_ERROR_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_DiagUserGet() Invalid SyncId=0x%x",
						SyncId);
	}
	else if ((GSY_TRIGGER_NOW != Trigger) && (GSY_TRIGGER_ALL != Trigger) 
		 &&  (GSY_TRIGGER_SYNC != Trigger) && (GSY_TRIGGER_CHECK != Trigger))
	{
		GSY_ERROR_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_DiagUserGet() Invalid Trigger=0x%x",
						Trigger);
	}
	else
	{
		RespVal = GSY_RSP_OK_ACTIVE;
		pDrift = &pChSys->Drift[SyncId][0];
		if (GSY_TRIGGER_CHECK == Trigger)
		{
			/* Copy input parameters to compare them
			 * 161007lrg003: AP00526517 RateValid and PrimaryMaster
			*/
			pDrift->Diag.Ind = GSY_UPPER_RQB_DIAGIND_PARAM_GET(pRbu);
		}
		if ((GSY_TRIGGER_NOW == Trigger)
		||  ((pDrift->Diag.Ind.DriftInterval != pDrift->Diag.DriftInterval) && (GSY_TRIGGER_ALL == Trigger))
		||   !GSY_MACADDR_CMP(pDrift->Diag.Ind.MasterMacAddr, pDrift->Diag.MasterMacAddr)
		||   ((GSY_DIAG_SUBDOMAIN_CONFIGURED == pDrift->Diag.Ind.Subdomain?LSA_TRUE:LSA_FALSE) != pDrift->Diag.Subdomain)
		||   ((GSY_DIAG_RATE_VALID == pDrift->Diag.Ind.RateValid?LSA_TRUE:LSA_FALSE) != pDrift->Diag.RateValid)
//		||   (pDrift->Diag.Ind.PrimaryMaster != pDrift->Diag.PrimaryMaster)		170408lrg002
		||   (pDrift->Diag.Ind.RcvSyncPrio != pDrift->Diag.RcvSyncPrio)
		||   (pDrift->Diag.Ind.LocalPrio != pDrift->Diag.LocalPrio)
		||   (pDrift->Diag.Ind.PLLState != pDrift->Diag.PLLState)
		||   ((GSY_DIAG_SYNCID_SYNCHRONIZED == pDrift->Diag.Ind.StateSync?LSA_TRUE:LSA_FALSE) != pDrift->Diag.StateSync))
		{
			GSY_UPPER_RQB_DIAGIND_SYNCID_SET(pRbu, SyncId);
			GSY_UPPER_RQB_DIAGIND_MASTERMAC_SET(pRbu, pDrift->Diag.MasterMacAddr);
			GSY_UPPER_RQB_DIAGIND_SUBDOMAIN_SET(pRbu, pDrift->Diag.Subdomain);
			GSY_UPPER_RQB_DIAGIND_STATESYNC_SET(pRbu, pDrift->Diag.StateSync);
			GSY_UPPER_RQB_DIAGIND_RATEVALID_SET(pRbu, pDrift->Diag.RateValid);
//			GSY_UPPER_RQB_DIAGIND_PRIMASTER_SET(pRbu, pDrift->Diag.PrimaryMaster);	170408lrg002
			GSY_UPPER_RQB_DIAGIND_RCVPRIO_SET(pRbu, pDrift->Diag.RcvSyncPrio);
			GSY_UPPER_RQB_DIAGIND_LOCPRIO_SET(pRbu, pDrift->Diag.LocalPrio);
			GSY_UPPER_RQB_DIAGIND_PLLSTATE_SET(pRbu, pDrift->Diag.PLLState);
			GSY_UPPER_RQB_DIAGIND_DRIFT_SET(pRbu, pDrift->Diag.DriftInterval);
			GSY_UPPER_RQB_DIAGIND_USER1_SET(pRbu, pDrift->Diag.UserValue1);
			GSY_UPPER_RQB_DIAGIND_USER2_SET(pRbu, pDrift->Diag.UserValue2);
			pDrift->Diag.Ind = GSY_UPPER_RQB_DIAGIND_PARAM_GET(pRbu);
			RespVal = GSY_RSP_OK;

			GSY_SYNC_TRACE_10(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_DiagUserGet(%02x) MAC=%02x-%02x-%02x Subdomain=%u StateSync=%u RateValid=%u RcvSyncPrio=0x%02x LocalPrio=0x%02x PLLState=%u",
							SyncId, pDrift->Diag.MasterMacAddr.MacAdr[3], pDrift->Diag.MasterMacAddr.MacAdr[4], pDrift->Diag.MasterMacAddr.MacAdr[5], pDrift->Diag.Subdomain, 
							pDrift->Diag.StateSync, pDrift->Diag.RateValid, pDrift->Diag.RcvSyncPrio, pDrift->Diag.LocalPrio, pDrift->Diag.PLLState);
		}
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_DiagUserGet() pRbu=%X Trigger=%u RespVal=0x%x",
						pRbu, Trigger, RespVal);
	return(RespVal);
}

/*****************************************************************************/
/* Internal function: gsy_DiagUserInd()                                      */
/* Store diagnosis event and indicate it to the user, if one RQB is queued.  */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_DiagUserInd(
LSA_INT					CallId,
GSY_CH_SYS_PTR			pChSys,
GSY_DRIFT_PTR			pDrift,
LSA_UINT32				MasterSeconds,
LSA_UINT32				MasterNanoseconds,
LSA_INT32				Offset,
LSA_INT32				AdjustInterval,
LSA_INT32				UserValue1,
LSA_INT32				UserValue2,
LSA_UINT32				SetTimeHigh,
LSA_UINT32				SetTimeLow,
LSA_UINT16				PortId)
{
	LSA_UINT16				RespVal;
	GSY_DIAG_DATA_TYPE		*pDiag;
	GSY_DIAG_DATA_TYPE		Diag;			// AP01368288 Compiler Warnings
	GSY_UPPER_RQB_PTR_TYPE  pRbu = LSA_NULL;
	LSA_BOOL				NewValue = LSA_FALSE;
	LSA_UINT8				SyncId = GSY_SYNCID_NONE;
	LSA_UINT8				ComClass = 0;
	LSA_UINT8				PLLState = GSY_PLL_STATE_SYNC_OFF;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_DiagUserInd() pChSys=%X pDrift=%X CallId=%u",
					pChSys, pDrift, CallId);

	/* Rate is not calculated on delay measurement "only":
	 * use local diagnosis structure
	*/
	if (LSA_HOST_PTR_ARE_EQUAL(pDrift, LSA_NULL))
	{
		GSY_MEMSET_LOCAL(&Diag, 0x00, sizeof(Diag));	// AP01368288 Compiler Warnings
		pDiag = &Diag;
	}
	else
	{
		pDiag = &pDrift->Diag;
		SyncId = pDrift->SyncId;

		/* 191006lrg002: always write MAC address to diagnosis buffer
		 * 030507lrg002: get MAC address from slave structure if synchronous
		 *               because more than one prim. master may send at the same time 
		 * 091007lrg001: from drift structure at primary Master
		*/
		if ((GSY_SYNCID_NONE != SyncId)
		&&  !LSA_HOST_PTR_ARE_EQUAL(pChSys->pChUsr, LSA_NULL))
		{
			if ((GSY_SLAVE_STATE_SYNC == pChSys->pChUsr->Slave.State)
			&&  (GSY_MASTER_STATE_PRIMARY != pChSys->pChUsr->Master.State))
				pDiag->MasterMacAddr = pChSys->pChUsr->Slave.MasterAddr;
			else
				pDiag->MasterMacAddr = pDrift->MasterAddr;

			if (GSY_MASTER_STATE_OFF != pChSys->pChUsr->Master.State)
			{
				/* 170408lrg002: at the Master LocalPrio is the projected  Priority1 with ActiveBit 
				*/
				pDrift->LocalPrio = pChSys->pChUsr->Master.LocalMaster.Priority1;

				if (GSY_MASTER_STATE_PRIMARY == pChSys->pChUsr->Master.State)
				{
					/* 170408lrg002: at the active Master is RcvSyncPrio 0
					*/
					pDrift->RcvSyncPrio = 0;
				}
			}
			else
			{
				/* 170408lrg002: LocalPrio is 0	at the slave
				 * RcvSyncPrio is extracted from SyncFrame in gsy_TLVGetInfo()
				*/
				pDrift->LocalPrio = 0;
			}
			PLLState = pChSys->pChUsr->PLLData.State;
		}
		else
		{
			/* 170408lrg002: After Stop of Master/Slave
			*/
			pDrift->RcvSyncPrio = 0;
			pDrift->LocalPrio = 0;
            if (!LSA_HOST_PTR_ARE_EQUAL(pChSys->pChUsr, LSA_NULL))
			{
	  			PLLState = pChSys->pChUsr->PLLData.State;
			}
		}

		/* 161007lrg003: AP00526517 RateValid and PrimaryMaster
		*/
		if (pDiag->RateValid != pDrift->IntervalSetValid)
		{
			NewValue = LSA_TRUE;
			pDiag->RateValid = pDrift->IntervalSetValid;
		}
//		if (pDiag->PrimaryMaster != pDrift->PrimaryMaster)	170408lrg002
		if (pDiag->LocalPrio != pDrift->LocalPrio)
		{
			NewValue = LSA_TRUE;
			pDiag->LocalPrio = pDrift->LocalPrio;
		}
		if (pDiag->RcvSyncPrio != pDrift->RcvSyncPrio)
		{
			NewValue = LSA_TRUE;
			pDiag->RcvSyncPrio = pDrift->RcvSyncPrio;
		}
	}

	/*  Store diagnosis event values if changed
	*/
	if (!LSA_HOST_PTR_ARE_EQUAL(pDrift, LSA_NULL)
	&&  (GSY_DRIFT_STATE_UNKNOWN != pDrift->State))
	{
		if (pDiag->DriftInterval != pDrift->AveInterval)
		{
			NewValue = LSA_TRUE;
			pDiag->DriftInterval = pDrift->AveInterval;
		}
	}
	else
	{
		if (pDiag->DriftInterval != 0)
		{
			NewValue = LSA_TRUE;
			pDiag->DriftInterval = 0;
		}
	}

	if (pDiag->PLLState != PLLState)
	{
		NewValue = LSA_TRUE;
		pDiag->PLLState = PLLState;
	}
	if (((Offset != 0) || (GSY_DIAG_SOURCE_OFFSET == CallId))
	&&  (pDiag->Offset != Offset))
	{
//		NewValue = LSA_TRUE;
		pDiag->Offset = Offset;
	}
	if (((AdjustInterval != 0) || (GSY_DIAG_SOURCE_ADJUST == CallId))
	&&  (pDiag->AdjustInterval != AdjustInterval))
	{
//		NewValue = LSA_TRUE;
		pDiag->AdjustInterval = AdjustInterval;
	}
	if ((UserValue1 != 0)
	&&  (pDiag->UserValue1 != UserValue1))
	{
//		NewValue = LSA_TRUE;
		pDiag->UserValue1 = UserValue1;
	}
	if ((UserValue2 != 0)
	&&  (pDiag->UserValue2 != UserValue2))
	{
//		NewValue = LSA_TRUE;
		pDiag->UserValue2 = UserValue2;
	}
	if (((MasterSeconds != 0) || (GSY_DIAG_SOURCE_OFFSET == CallId))
	&&  (pDiag->MasterSeconds != MasterSeconds))
	{
		pDiag->MasterSeconds = MasterSeconds;
	}
	if (((MasterNanoseconds != 0) || (GSY_DIAG_SOURCE_OFFSET == CallId))
	&&  (pDiag->MasterNanoseconds != MasterNanoseconds))
	{
		pDiag->MasterNanoseconds = MasterNanoseconds;
	}
	
	if (!LSA_HOST_PTR_ARE_EQUAL(pDrift, LSA_NULL) 
	&&  (NewValue
	 ||  (GSY_DIAG_SOURCE_SYNC == CallId)
	 ||  (GSY_DIAG_SOURCE_SUBDOMAIN == CallId)
	 ||  (GSY_DIAG_SOURCE_MASTER == CallId)))
	{
		if (GSY_SYNCID_NONE == SyncId)
		{
			/* Not yet received any SyncFrame
			*/
			SyncId = pDiag->SyncId;
			if ((GSY_SYNCID_NONE == SyncId)
			&&  !LSA_HOST_PTR_ARE_EQUAL(pChSys->pChUsr, LSA_NULL))	//1365226
			{
				SyncId = pChSys->pChUsr->SyncId;
			}
		}

		/* Callback user if a RQB is queued
		 * and a diagnosis value has changed:
		*/
		if (GSY_SYNCID_NONE != SyncId)
		{
			pRbu = pChSys->QueueDiag[SyncId].pFirst;
			if (!LSA_HOST_PTR_ARE_EQUAL(pRbu, LSA_NULL))
			{
				RespVal = gsy_DiagUserGet(pChSys, pRbu);
				if (GSY_RSP_OK_ACTIVE != RespVal)
				{
	        		gsy_GetUpperQ(&pChSys->QueueDiag[SyncId], pRbu);
					GSY_UPPER_RQB_DIAGIND_INDCNT_SET(pRbu, pChSys->QueueDiag[SyncId].Count);
					gsy_CallbackUsr(pChSys->pChUsr, pRbu, RespVal);
				}
			}
		}
		else
		{
			GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** gsy_DiagUserInd() pChSys=%X pDrift=%X CallId=%u: No SyncId found",
				pChSys, pDrift, CallId);
		}
	}

/*	Always update diagnosis record !
	if (NewValue || (GSY_DIAG_SOURCE_DELAY == CallId) || (GSY_DIAG_SOURCE_SET == CallId)) 
	061206lrg001 Do not write "new" diagnosis sources into DiagRecord */
	if ((GSY_DIAG_SOURCE_SYNC != CallId)
	&&  (GSY_DIAG_SOURCE_SUBDOMAIN != CallId)
	&&  (GSY_DIAG_SOURCE_MASTER != CallId))
	{
		/* 130606lrg002: not only if any diagnosis value has changed 
		 *               or Delay/ExtPllOffset is written:
		 * Write Diag structure++ into diagnosis record(s)
		*/
		/* Diagnosis record of the user channel[SyncId] 
		*/
		if ((GSY_SYNCID_TIME != SyncId)							//Task 1311090: 0xB001 not longer supported
		&&  !LSA_HOST_PTR_ARE_EQUAL(pChSys->pChUsr, LSA_NULL))
		{
			gsy_PrmDiagWrite(pChSys->pChUsr, pDiag, SetTimeHigh, SetTimeLow, PortId, GSY_SYNCID_CLOCK, ComClass, CallId);
		}
	}
	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_DiagUserInd() NewValue=%d MasterSeconds=%u MasterNanos=%u",
					NewValue, MasterSeconds, MasterNanoseconds);
}

/*****************************************************************************/
/* Internal function: gsy_SyncParams()                                       */
/* Read working and statistic values of the synchronisation                  */
/*****************************************************************************/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_SyncParams(
GSY_CH_USR_PTR			pChUsr,
GSY_UPPER_RQB_PTR_TYPE	pRbu)
{
	LSA_UINT32	TimeStamp = 0, Seconds = 0, Nanos = 0;
	LSA_UINT16	Epoch = 0, RetVal = LSA_RET_OK;
	LSA_UINT16	RespVal = GSY_RSP_ERR_PARAM;
	GSY_RQB_SYNC_GET_PARAMS_PTR_TYPE pSyncParams = GSY_UPPER_RQB_GET_PARAMS_PTR_GET(pRbu);

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_SyncParams() pChUsr=%X pRbu=%X pSyncParams=%X",
					pChUsr, pRbu, pSyncParams);

	if (GSY_SYNCID_NONE == pChUsr->SyncId)
	{
		RespVal = GSY_RSP_ERR_SEQUENCE;
	}
	else
	{
		/* If a sync record is loaded...
		*/
		if (!LSA_HOST_PTR_ARE_EQUAL(pSyncParams, LSA_NULL))
		{
			if 	((GSY_HW_TYPE_SLAVE_SW == (pChUsr->pChSys->HardwareType & GSY_HW_TYPE_SLAVE_MSK))
			&&   ((GSY_SLAVE_STATE_WAIT_SET == pChUsr->Slave.State) 
			 ||  (GSY_PLL_STATE_SYNC_WAIT_SET == pChUsr->PLLData.State)))
			{
				/* GSY_SYNC_GET() not allowed now: 
				 * finish this function at SYNC_SET-Confirmation in gsy_Rtc2Cnf()
				*/
				gsy_PutUpperQ(&pChUsr->QueueWait, pRbu);
				RespVal = GSY_RSP_OK_ACTIVE;
			}
			else
			{
				/* 100209lrg001: do not get sync time on EDDP SyncChannel
				*/
				if (GSY_HW_TYPE_SLAVE_SW == (pChUsr->pChSys->HardwareType & GSY_HW_TYPE_SLAVE_MSK))
				{
					/* Aktuelle Zeit holen
					*/
					RetVal = GSY_SYNC_GET(&Epoch, &Seconds, &Nanos, &TimeStamp, pChUsr->SyncId, pChUsr->pSys);
				}
				if (LSA_RET_OK == RetVal)
				{
					/* Calculate age for statistic data
					*/
					Seconds -= pChUsr->Statistics.StatisticAge.Seconds;
					if (Seconds == 0)
						Nanos -= pChUsr->Statistics.StatisticAge.Nanoseconds;
					else
					{
						Nanos += (GSY_NANOS_PER_SECOND - pChUsr->Statistics.StatisticAge.Nanoseconds);
						Seconds--;
						while (Nanos >= GSY_NANOS_PER_SECOND)
						{
							Nanos -= GSY_NANOS_PER_SECOND;
							Seconds++;
						}
					}
					/* Write statistic data and age to parameter buffer
					*/
					GSY_MEMSET_UPPER(pSyncParams, 0, sizeof(GSY_RQB_SYNC_GET_PARAMS_TYPE));
					pSyncParams->Statistics = pChUsr->Statistics;
					pSyncParams->Statistics.StatisticAge.Seconds = Seconds;
					pSyncParams->Statistics.StatisticAge.Nanoseconds = Nanos;

					/* Write Sync parameters
					*/
					if (GSY_SYNC_ROLE_MASTER == pChUsr->SyncRole)
						pSyncParams->RoleIsSyncMaster = GSY_SYNC_ROLE_IS_SYNCMASTER;
					else
						pSyncParams->RoleIsSyncMaster = GSY_SYNC_ROLE_IS_NOT_SYNCMASTER;
					pSyncParams->PLLWindow = pChUsr->Slave.WindowSize;

					/* 210807lrg001: Timeout is counter instead of multiple of 3125ns
					pSyncParams->PTCPTimeoutFactor = pChUsr->Slave.SyncTimeout;
					*/
					if ((GSY_MASTER_STATE_OFF != pChUsr->Master.State)
					||  (GSY_SLAVE_STATE_OFF != pChUsr->Slave.State))
						pSyncParams->SyncActivity = GSY_SYNC_SYNCACTIVITY_ACTIVE;
					else
						pSyncParams->SyncActivity = GSY_SYNC_SYNCACTIVITY_NO_SERVICE;
					if (GSY_SYNC_SYNCACTIVITY_ACTIVE == pSyncParams->SyncActivity)
					{
						pSyncParams->SyncInTime = pChUsr->Slave.SyncInTime;
						pSyncParams->ControlPeriod = pChUsr->Slave.SyncInterval * GSY_CLOCK_BASE_PER_MILLISECOND;
						pSyncParams->PTCPTimeoutFactor = pChUsr->Slave.SyncTimeout * pSyncParams->ControlPeriod;
					}
					RespVal = GSY_RSP_OK;
				}
			}
		}
	}

	GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_SyncParams() pRbu=%X RespVal=0x%x",
					pRbu, RespVal);
	return(RespVal);
}

/*****************************************************************************/
/* Internal function: gsy_SyncStop()                                         */
/* Stop a master, PLLsync and/or slave                                       */
/*****************************************************************************/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_SyncStop(
GSY_CH_USR_PTR			pChUsr,
GSY_UPPER_RQB_PTR_TYPE	pRbu)
{
	LSA_UINT16	RespVal = GSY_RSP_ERR_SEQUENCE;
	LSA_BOOL	IndSync = LSA_TRUE;

	GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelchat*/,"  > gsy_SyncStop() pChUsr=%X pRbu=%X",
					pChUsr, pRbu);

	if ((GSY_MASTER_STATE_OFF != pChUsr->Master.State)
	||  (GSY_SLAVE_STATE_OFF != pChUsr->Slave.State))
	{
		/* If there is a master running on this channel...
		*/
		if (GSY_MASTER_STATE_OFF != pChUsr->Master.State)
		{
			/* All will be done from gsy_MasterSend() at next MasterTimer
			*/
			GSY_SYNC_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_SyncStop(%02x) master state change %u->STOP", pChUsr->SyncId, pChUsr->Master.State);
			pChUsr->Master.State = GSY_MASTER_STATE_STOP;
		}

		/* If there is a slave running on this channel...
		*/
		if (GSY_SLAVE_STATE_OFF != pChUsr->Slave.State)
		{
			/* All will be done at next SlaveTimer
			*/
			GSY_SYNC_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_SyncStop(%02x) slave state change %u->STOP", pChUsr->SyncId, pChUsr->Slave.State);
			pChUsr->Slave.State = GSY_SLAVE_STATE_STOP;
		}

		/* If the sync is running in hardware...
		*/
		if ((GSY_HW_TYPE_SLAVE_HW == (pChUsr->pChSys->HardwareType & GSY_HW_TYPE_SLAVE_MSK))
		&&  ((GSY_SYNCID_CLOCK == pChUsr->SyncId) || (GSY_SYNCID_TIME == pChUsr->SyncId)))
		{
			/* Stop KRISC32 slave and master by sending a RQB to EDDP
			*/
			GSY_MAC_ADR_TYPE	NoMacAddr = {GSY_NO_MASTER_ADDR};

			pChUsr->Event.PriMasterWaits = LSA_FALSE;
			pChUsr->Event.Prm.Seconds = 0;
			pChUsr->Event.Prm.Nanoseconds = 0;
			pChUsr->Event.OldPrm.ExtChannelErrorType = GSY_PRM_EVENT_NONE;
			pChUsr->Event.PrmFlags = GSY_PRM_IND_FLAGS_NONE;
			pChUsr->Event.SyncEvent = GSY_SYNC_EVENT_NONE; 
			pChUsr->Slave.SyncError = GSY_SYNC_EVENT_NO_MESSAGE_RECEIVED;
			pChUsr->Slave.MasterAddr = NoMacAddr;
			pChUsr->Slave.AdjustSeconds = 0;
			pChUsr->Slave.AdjustNanos = 0;
			pChUsr->Slave.SeqId = 0;
			pChUsr->Slave.SyncPortId = 0;
			pChUsr->Slave.WaitFU = LSA_FALSE;
			pChUsr->Slave.SumTimeout = 0;
			pChUsr->Slave.AdjustTermP = 0;
			pChUsr->Slave.AdjustTermI = 0;

			gsy_MasterSlaveControl(pChUsr, GSY_LOWER_SLAVE_STOP, GSY_LOWER_MASTER_OFF);
			IndSync = LSA_FALSE;
		}
		else
		{
#ifdef GSY_CFG_SYNC_ONLY_IN_KRISC32
			gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_CONFIG, LSA_NULL, 0);
#else
			/* 160511lrg: start slave timer to stop slave
			*/
			if (GSY_SLAVE_STATE_STOP == pChUsr->Slave.State)
			{
				gsy_SlaveTimerStart(pChUsr, LSA_FALSE);
			}
#endif //GSY_CFG_SYNC_ONLY_IN_KRISC32
		}

		/* Deactivate receiving of Announce, Sync and FollowUp Frames
		*/
		RespVal = gsy_FwdUserForwardCtrl(pChUsr->pChSys, GSY_FWD_MODE_DISABLE, 
										(LSA_UINT32)pChUsr->SyncId);

		if (GSY_RSP_OK == RespVal)
		{
			RespVal = GSY_RSP_OK_ACTIVE;
	        gsy_PutUpperQ(&pChUsr->QueueUpper, pRbu);
			pChUsr->Event.SyncEvent = GSY_SYNC_EVENT_NO_MESSAGE_RECEIVED; 
		}
	}

	if (IndSync 
	&& ((GSY_MASTER_STATE_STOP == pChUsr->Master.State)
	 || (GSY_SLAVE_STATE_STOP == pChUsr->Slave.State)))		/* 040907lrg002: also at GSY_PRM_STATE_STOP
	&&  (pChUsr->PrmState != GSY_PRM_STATE_STOP))			190706lrg001 */
	{
		/*
		 * 040907lrg001: signal NotSync to Lower Layer
		*/
		gsy_SyncInd(pChUsr, LSA_FALSE);
	}

	GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelchat*/,"  < gsy_SyncStop() pRbu=%X RespVal=0x%x",
					pRbu, RespVal);
	return(RespVal);
}


/*****************************************************************************/
/* Internal function: gsy_SyncInd()                                          */
/* Signal Sync oder NotSync to lower layer, system and/or user               */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_SyncInd(
GSY_CH_USR_PTR			pChUsr,
LSA_BOOL				Sync)
{
	GSY_LOWER_RQB_PTR_TYPE	pRbl = LSA_NULL;
	GSY_DRIFT_PTR 			pDrift = &pChUsr->pChSys->Drift[pChUsr->SyncId][0];

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_SyncInd() pChUsr=%X SyncId=0x%02x Sync=%d",
					pChUsr, pChUsr->SyncId, Sync);

	if (GSY_SYNCID_CLOCK == pChUsr->SyncId)
	{
		/* 250708lrg001: also signal only change to the system
		 * but with NOT sync as start state
		*/
		if (pChUsr->InfoSync != Sync)
		{
			pChUsr->InfoSync = Sync;
		}
		/* 040907lrg001: signal clock sync change to Lower Layer
		*/
		if ((!pChUsr->RTC2Active && Sync) || (pChUsr->RTC2Active && !Sync))
		{
			if (0 != pChUsr->qRTC2Rbl.Count)
			{
				/* Get Lower RQB from Queue
				*/
				pRbl = pChUsr->qRTC2Rbl.pFirst;
				gsy_DequeLower(&pChUsr->qRTC2Rbl, pRbl);
			}
			else
			{
				/* Allocate Lower RQB
				*/
				if (GSY_RET_OK == gsy_AllocEthParam(pChUsr->pChSys, &pRbl, LSA_FALSE))
					pChUsr->RTC2RblCount++;
			}

			if  (!LSA_HOST_PTR_ARE_EQUAL(pRbl, LSA_NULL))
			{
				/* Fill and send RQB
				*/
				GSY_LOWER_RQB_OPCODE_SET(pRbl, GSY_LOWER_OPC_REQUEST);
                GSY_LOWER_RQB_HANDLE_SET(pRbl, 0); /* sanity */
				GSY_LOWER_RQB_LOWER_HANDLE_SET(pRbl, pChUsr->pChSys->HandleLower);
				GSY_LOWER_RQB_SERVICE_SET(pRbl, GSY_LOWER_SYNC_SET);
				GSY_LOWER_RQB_ERR_COMPID_SET(pRbl, LSA_COMP_ID_UNUSED);
				GSY_LOWER_RQB_USER_PTR_SET(pRbl, LSA_NULL);

				if (!pChUsr->RTC2Active)
				{
					GSY_LOWER_RQB_SET_STATE_SYNC_SET(pRbl, GSY_SYNC_STATE_GOOD);
					pChUsr->RTC2Active = LSA_TRUE;
				}
				else
				{
					GSY_LOWER_RQB_SET_STATE_SYNC_SET(pRbl, GSY_SYNC_STATE_BAD);
					pChUsr->RTC2Active = LSA_FALSE;
				}
	            GSY_SYNC_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_SyncInd() Lower set SyncState=%d", 
	            				pChUsr->RTC2Active ? GSY_SYNC_STATE_GOOD:GSY_SYNC_STATE_BAD);

				GSY_REQUEST_LOWER(pRbl, pChUsr->pChSys->pSys);
			}
			else
				/* No RQB in Queue and no more memory
				*/
				gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_NO_RBL, pChUsr, 0);
		}
	}

	/* 070108lrg003: OHA signals error if Diag.Subdomain is set and no sync record is loaded
	*/
	if (GSY_SUBDOMAIN_IS_NULL(pChUsr->SyncData.SubdomainID))
	{
		pDrift->Diag.Subdomain = LSA_FALSE;
	}
	/* 221106lrg001: update diagnosis data and send Indication
	*/
	pDrift->Diag.StateSync = Sync;
	gsy_DiagUserInd(GSY_DIAG_SOURCE_SYNC, pChUsr->pChSys, pDrift, 
	    			0      /* LSA_UINT32: MasterSeconds */, 
					0      /* LSA_UINT32: MasterNanoseconds */, 
					0      /* LSA_INT32: Offset */, 
					0      /* LSA_INT32: AdjustInterval */, 
					0      /* LSA_INT32: UserValue1 */, 
					0      /* LSA_INT32: UserValue2 */, 
					0      /* LSA_INT32: SetTimeHigh */, 
					0      /* LSA_INT32: SetTimeLow */, 
					0	   /* LSA_INT16: PortId */);

	GSY_FUNCTION_TRACE_00(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_SyncInd()");
}

/*****************************************************************************/
/* Internal function: gsy_SyncUserInd()                                      */
/* Store time of the actual Sync Event and signal the last or actual event   */
/* to the user if a upper RQB is in queue.                                   */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_SyncUserInd(
GSY_CH_USR_PTR			pChUsr,
LSA_BOOL				bSetTime,
LSA_BOOL				bSetEvent)
{
	LSA_UINT32	TimeStamp = 0;
	LSA_UINT16	Epoch = 0;
	LSA_UINT16	PrmIndCnt = 0;
	LSA_UINT16	SyncEvent = pChUsr->Event.SyncEvent;
	LSA_UINT16	RetVal = LSA_RET_OK;
	LSA_UINT16	Response = GSY_RSP_OK;
	GSY_UPPER_RQB_PTR_TYPE  	pRbu = LSA_NULL;

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_SyncUserInd() pChUsr=%X Event=%d SlaveError=%d",
					pChUsr, SyncEvent, pChUsr->Slave.SyncError);

	if (bSetTime)
	{
		pChUsr->Event.Prm.Seconds = 0;		//AP01361288 
		pChUsr->Event.Prm.Nanoseconds = 0;

		/* 100209lrg001: do not get time on EDDP SyncChannel
		*/
		if ((GSY_HW_TYPE_SLAVE_SW == (pChUsr->pChSys->HardwareType & GSY_HW_TYPE_SLAVE_MSK)) && (GSY_SYNCID_NONE != pChUsr->SyncId))
		{
			/* EventTime = now: read actual time with stamp from system
			*/
			RetVal = GSY_SYNC_GET(&Epoch, &pChUsr->Event.Prm.Seconds, &pChUsr->Event.Prm.Nanoseconds, &TimeStamp, pChUsr->SyncId, pChUsr->pSys);
			if (LSA_RET_OK != RetVal)
			{
				GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_SyncUserInd() pChUsr=%X SyncId=%u: GSY_SYNC_GET: RetVal=0x%x",
								pChUsr, pChUsr->SyncId, RetVal);
			}
		}
	}

	if (bSetEvent)
	{
		/* Update PRM Indication structure
		*/
		PrmIndCnt = gsy_PrmEventSet(pChUsr, SyncEvent);

		/* Interface-Event
		*/
		pChUsr->Event.PortId = 0;

	}

	/* 040907lrg001: signal Sync to Lower Layer 210907lrg002: in SyncInd()
	LSA_UNUSED_ARG(SetRtc2);
	*/

	if ((GSY_MASTER_STATE_STOP != pChUsr->Master.State)
	&&  (GSY_SLAVE_STATE_STOP != pChUsr->Slave.State)
	&&  ((GSY_SYNC_EVENT_NONE == pChUsr->Event.SyncEvent)		// 041006lrg001: arbitrarily often (PrmCommit w/o PrmWrite)
	  || (SyncEvent != pChUsr->Event.OldEvent) 
	  || (pChUsr->Event.PortId != pChUsr->Event.OldPort)))
	{
		/* If an upper RQB is in the Indication-Queue
		 * and Master/Slave is not stopping:
		 * indicate event or error to user
		*/
		if ((pChUsr->QueuePrm.Count != 0) && (0 != PrmIndCnt))
		{
			/* Get PRM Indication RQB from Queue and fill in the values
			*/
			pRbu = pChUsr->QueuePrm.pFirst;
	        gsy_DequeUpper(&pChUsr->QueuePrm, pRbu);
			(LSA_VOID)gsy_PrmUserInd(pChUsr, pRbu);
   		}
		else
		{
			GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_WARN,"*** gsy_SyncUserInd() pChUsr=%X Event=%d SyncId=%u: NO USER INDICATION RQB",
						pChUsr, SyncEvent, pChUsr->SyncId);
		}

		pChUsr->Event.OldEvent = SyncEvent;
		pChUsr->Event.OldPort = pChUsr->Event.PortId;

		/* If no lower Request is pending (120207lrg001: or just running): send upper Indication
		*/
		if (!LSA_HOST_PTR_ARE_EQUAL(pRbu, LSA_NULL))
		{
			/* 120207lrg001: PRM-Indication RQB wats for lower Confirmation
			*/
			if (pChUsr->QueueEvent.Count != 0)
		        gsy_PutUpperQ(&pChUsr->QueueEvent, pRbu);
			else
				gsy_CallbackUsr(pChUsr, pRbu, Response);
		}
	}

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_SyncUserInd() pRbu=%X EventSeconds=%u EventNanos=%u",
					pRbu, pChUsr->Event.Prm.Seconds, pChUsr->Event.Prm.Nanoseconds);
}

/*****************************************************************************/
/* Internal function: gsy_PrmEventSet()                                      */
/* Store actual Sync Event in the PRM Indication structure                   */
/*****************************************************************************/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_PrmEventSet(
GSY_CH_USR_PTR			pChUsr,
LSA_UINT16				SyncEvent)
{
	LSA_UINT16	PrmIndCnt = 0;

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_PrmEventSet() pChUsr=%X Event=%d PrmFlags=0x%02x",
					pChUsr, SyncEvent, pChUsr->Event.PrmFlags);

	/* Update PRM Indication Structure
	*/

	switch (SyncEvent)
	{
	case GSY_SYNC_EVENT_DELAY_ERROR:
	case GSY_SYNC_EVENT_DELAY_OK:
	case GSY_SYNC_EVENT_WRONG_PTCP_SUBDOMAIN_ID:

		/* 150306lrg001: not longer signaled in 3.11
		*/
#ifdef GSY_MESSAGE
		/* 090806js001: this case must not longer happen. 
		 * Else one indication will be missed after PrmCommit
		 * because nothing is done here. Do not longer write 
		 * these events to SyncEvent.
		*/
#endif  /* GSY_MESSAGE */

		GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_SyncUserInd() pChUsr=%X SyncId=%u: Unexpected event=%u",
						pChUsr, pChUsr->SyncId, SyncEvent);
		break;

	case GSY_SYNC_EVENT_NO_MESSAGE_RECEIVED:
		pChUsr->Event.Prm.ExtChannelErrorType = GSY_PRM_EVENT_NO_MESSAGE_RECEIVED;
		pChUsr->Event.Prm.ChannelProperties = GSY_PRM_CHANNEL_PROPERTIES|GSY_PRM_EVENT_APPEARS;
		pChUsr->Event.PrmFlags |= GSY_PRM_IND_FLAGS_TIMEOUT;
		pChUsr->Event.NewOldPrm = LSA_FALSE;
		if (!(pChUsr->Event.IndFlags & GSY_PRM_IND_FLAGS_TIMEOUT))
		{
			pChUsr->Event.NewPrm = LSA_TRUE;
			PrmIndCnt = 1;
			if (pChUsr->Event.IndFlags & GSY_PRM_IND_FLAGS_NOTSYNC)
			{
				/* 260207lrg001: GSY_PRM_EVENT_JITTER_OUT_OF_BOUNDARY before Timeout:
				 * now indicate "NotSync disappearing" before "Timeout appearing"
				*/
				pChUsr->Event.OldPrm = pChUsr->Event.Prm;
				pChUsr->Event.OldPrm.ExtChannelErrorType = GSY_PRM_EVENT_JITTER_OUT_OF_BOUNDARY;
				pChUsr->Event.OldPrm.ChannelProperties = GSY_PRM_CHANNEL_PROPERTIES|GSY_PRM_EVENT_DISAPPEARS;
				pChUsr->Event.NewOldPrm = LSA_TRUE;
				PrmIndCnt = 2;
				pChUsr->Event.PrmFlags &= ~GSY_PRM_IND_FLAGS_NOTSYNC;
			}
		}
		break;

	case GSY_SYNC_EVENT_JITTER_OUT_OF_BOUNDARY:
		pChUsr->Event.NewOldPrm = LSA_FALSE;
		if (pChUsr->Event.PrmFlags & GSY_PRM_IND_FLAGS_TIMEOUT)
		{
			/* Getting synchronous after Timeout or Start:
			*/
			if (pChUsr->Event.PrmFlags & GSY_PRM_IND_FLAGS_NOTSYNC)
			{
				if (pChUsr->Event.IndFlags & GSY_PRM_IND_FLAGS_TIMEOUT)
				{
					/* GSY_PRM_EVENT_JITTER_OUT_OF_BOUNDARY before Timeout:
					 * only indicate "Timeout disappearing" if it has been indicated as appearing before
					*/
					pChUsr->Event.Prm.ExtChannelErrorType = GSY_PRM_EVENT_NO_MESSAGE_RECEIVED;
					pChUsr->Event.Prm.ChannelProperties = GSY_PRM_CHANNEL_PROPERTIES|GSY_PRM_EVENT_DISAPPEARS;
					pChUsr->Event.NewPrm = LSA_TRUE;
					PrmIndCnt = 1;
				}
			}
			else
			{
				/* No GSY_PRM_EVENT_JITTER_OUT_OF_BOUNDARY before Timeout:
				 * max. indicate "Timeout disappearing" and "NotSync appearing" and set Flag
				*/
				if ((pChUsr->Event.IndFlags & GSY_PRM_IND_FLAGS_TIMEOUT)
				&&  (!(pChUsr->Event.IndFlags & GSY_PRM_IND_FLAGS_NOTSYNC)))
				{
					pChUsr->Event.OldPrm = pChUsr->Event.Prm;
					pChUsr->Event.OldPrm.ExtChannelErrorType = GSY_PRM_EVENT_NO_MESSAGE_RECEIVED;
					pChUsr->Event.OldPrm.ChannelProperties = GSY_PRM_CHANNEL_PROPERTIES|GSY_PRM_EVENT_DISAPPEARS;
					pChUsr->Event.Prm.ExtChannelErrorType = GSY_PRM_EVENT_JITTER_OUT_OF_BOUNDARY;
					pChUsr->Event.Prm.ChannelProperties = GSY_PRM_CHANNEL_PROPERTIES|GSY_PRM_EVENT_APPEARS;
					pChUsr->Event.NewOldPrm = LSA_TRUE;
					pChUsr->Event.NewPrm = LSA_TRUE;
					PrmIndCnt = 2;
				}
				else if (pChUsr->Event.IndFlags & GSY_PRM_IND_FLAGS_TIMEOUT)
				{
					pChUsr->Event.Prm.ExtChannelErrorType = GSY_PRM_EVENT_NO_MESSAGE_RECEIVED;
					pChUsr->Event.Prm.ChannelProperties = GSY_PRM_CHANNEL_PROPERTIES|GSY_PRM_EVENT_DISAPPEARS;
					pChUsr->Event.NewPrm = LSA_TRUE;
					PrmIndCnt = 1;
				}
				else if (!(pChUsr->Event.IndFlags & GSY_PRM_IND_FLAGS_NOTSYNC))
				{
					pChUsr->Event.Prm.ExtChannelErrorType = GSY_PRM_EVENT_JITTER_OUT_OF_BOUNDARY;
					pChUsr->Event.Prm.ChannelProperties = GSY_PRM_CHANNEL_PROPERTIES|GSY_PRM_EVENT_APPEARS;
					pChUsr->Event.NewPrm = LSA_TRUE;
					PrmIndCnt = 1;
				}
			}
			/* Reset Timeout PrmIndFlag
			*/
			pChUsr->Event.PrmFlags &= ~GSY_PRM_IND_FLAGS_TIMEOUT;
		}
		else if (!(pChUsr->Event.IndFlags & GSY_PRM_IND_FLAGS_NOTSYNC))
		{
			/* synchronisation lost and not indicated yet:
			*/
			pChUsr->Event.Prm.ExtChannelErrorType = GSY_PRM_EVENT_JITTER_OUT_OF_BOUNDARY;
			pChUsr->Event.Prm.ChannelProperties = GSY_PRM_CHANNEL_PROPERTIES|GSY_PRM_EVENT_APPEARS;
			pChUsr->Event.NewPrm = LSA_TRUE;
			PrmIndCnt = 1;
		}
		pChUsr->Event.PrmFlags |= GSY_PRM_IND_FLAGS_NOTSYNC;
		break;

	case GSY_SYNC_EVENT_SYNC_OK:

		/* 150306lrg001: is indicated in 3.11 as "old event disappearing" and all PrmFlags are reset
		*/
		pChUsr->Event.NewOldPrm = LSA_FALSE;
		if (pChUsr->Event.IndFlags & GSY_PRM_IND_FLAGS_NOTSYNC)
		{
			/* NotSync has been indicated...
			*/
			pChUsr->Event.Prm.ExtChannelErrorType = GSY_PRM_EVENT_JITTER_OUT_OF_BOUNDARY;
			pChUsr->Event.Prm.ChannelProperties = GSY_PRM_CHANNEL_PROPERTIES|GSY_PRM_EVENT_DISAPPEARS;
			pChUsr->Event.NewPrm = LSA_TRUE;
			PrmIndCnt = 1;
			if (pChUsr->Event.IndFlags & GSY_PRM_IND_FLAGS_TIMEOUT)
			{
				/* Timeout has been indicated also...
				*/
				pChUsr->Event.OldPrm = pChUsr->Event.Prm;
				pChUsr->Event.OldPrm.ExtChannelErrorType = GSY_PRM_EVENT_NO_MESSAGE_RECEIVED;
				pChUsr->Event.NewOldPrm = LSA_TRUE;
				PrmIndCnt++;	
			}
		}
		else if (pChUsr->Event.IndFlags & GSY_PRM_IND_FLAGS_TIMEOUT)								
		{
			/* Only Timeout has been indicated...
			*/
			pChUsr->Event.Prm.ExtChannelErrorType = GSY_PRM_EVENT_NO_MESSAGE_RECEIVED;
			pChUsr->Event.Prm.ChannelProperties = GSY_PRM_CHANNEL_PROPERTIES|GSY_PRM_EVENT_DISAPPEARS;
			pChUsr->Event.NewPrm = LSA_TRUE;
			PrmIndCnt = 1;
		}
		else if ((pChUsr->Event.IndFlags == GSY_PRM_IND_FLAGS_NONE)
		     &&	 (pChUsr->Event.PrmFlags == GSY_PRM_IND_FLAGS_NONE))
		{
			/* 030506lrg001: nothing has been indicated...
			*/
			pChUsr->Event.Prm.ExtChannelErrorType = GSY_PRM_EVENT_NONE;
			pChUsr->Event.NewPrm = LSA_TRUE;
			PrmIndCnt = 1;
		}
		pChUsr->Event.PrmFlags = GSY_PRM_IND_FLAGS_NONE;
		break;

	case GSY_SYNC_EVENT_NONE:

		/* 070706lrg001: nothing to indicate at FirstIndication
		*/
		pChUsr->Event.NewOldPrm = LSA_FALSE;
		pChUsr->Event.Prm.ExtChannelErrorType = GSY_PRM_EVENT_NONE;
		pChUsr->Event.NewPrm = LSA_TRUE;
		PrmIndCnt = 1;
		pChUsr->Event.PrmFlags = GSY_PRM_IND_FLAGS_NONE;
		break;

	default:
		GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** gsy_PrmEventSet() pChUsr=%X SyncId=%u: Unkwown event=%u",
						pChUsr, pChUsr->SyncId, SyncEvent);
	}

	GSY_PRM_TRACE_05(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PrmEventSet(%02x) Event=%u IndFlags=0x%02x PrmFlags=0x%02x PrmIndCnt=%d",
					pChUsr->SyncId, SyncEvent, pChUsr->Event.IndFlags, pChUsr->Event.PrmFlags, PrmIndCnt);

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_PrmEventSet() IndFlags=0x%02x PrmFlags=0x%02x PrmIndCnt=%u",
					 pChUsr->Event.IndFlags, pChUsr->Event.PrmFlags, PrmIndCnt);

	return(PrmIndCnt);
}

/*****************************************************************************/
/* Internal function: gsy_PrmDiagWrite()                                     */
/* Write diagnosis block to diagnosis record                                 */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_PrmDiagWrite(
GSY_CH_USR_PTR		pChUsr,
GSY_DIAG_DATA_TYPE	*pDiag,
LSA_UINT32			SetTimeHigh,
LSA_UINT32			SetTimeLow,
LSA_UINT16			PortId,
LSA_UINT8			SyncId,
LSA_UINT8			ComClass,
LSA_INT				CallId)
{
	LSA_UINT32	SyncDelay;
	LSA_UINT32	RecvDelay = pChUsr->Slave.Act.IndTimestamp - pChUsr->Slave.Act.Timestamp;
	LSA_UINT32	TimeStamp = 0;
	LSA_INT32	ExtPllOffset = GSY_GET_EXT_PLL_OFFSET(SyncId, pChUsr->pSys);
	GSY_DIAG_DATA_BLOCK	*pDiagBlock = &pChUsr->DiagRecord.DiagBlock[pChUsr->DiagIdxWr];

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_PrmDiagWrite() pChUsr=%X SyncId=0x%02x CallId=%d",
					pChUsr, SyncId, CallId);

	if (GSY_HW_TYPE_SLAVE_SW == (pChUsr->pChSys->HardwareType & GSY_HW_TYPE_SLAVE_MSK))
		TimeStamp = GSY_GET_TIMESTAMP(pChUsr->pChSys->pSys);

	if (PortId != 0)
		pDiagBlock->LineDelay.Dword = GSY_SWAP32(pChUsr->pChSys->Port[PortId-1].SyncLineDelay);
	else
		pDiagBlock->LineDelay.Dword = 0;

	GSY_DIAG_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"  > gsy_PrmDiagWrite(%x-%u) DiagIdxWr=%02u DiagNo=%05u",
					pChUsr, CallId, pChUsr->DiagIdxWr, pChUsr->DiagNo);

	if (RecvDelay > GSY_NANOS_PER_DWORD / pChUsr->pChSys->RxTxNanos)
		RecvDelay = 0xffffffff;
	else
		RecvDelay *= pChUsr->pChSys->RxTxNanos;

	if ((pChUsr->Slave.Act.DelaySeconds > GSY_SECONDS_PER_DWORD)
	||  ((pChUsr->Slave.Act.DelaySeconds == GSY_SECONDS_PER_DWORD)
	 &&  (pChUsr->Slave.Act.DelayNanos >= GSY_MAX_NANOS)))
		SyncDelay = 0xffffffff;
	else
	{
		SyncDelay = pChUsr->Slave.Act.DelaySeconds * GSY_NANOS_PER_SECOND;
		SyncDelay += pChUsr->Slave.Act.DelayNanos;
	}

	GSY_DIAG_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"  - gsy_PrmDiagWrite() TimeStamp=%u SyncDelay=%u RecvDelay=%u ExtPllOffset=%d",
					TimeStamp, SyncDelay, RecvDelay, ExtPllOffset);

	pDiagBlock->ClpTermPHigh.Dword = GSY_SWAP32((LSA_UINT32)((LSA_UINT64)(pChUsr->Slave.AdjustTermP) >> 32));
	pDiagBlock->ClpTermPLow.Dword = GSY_SWAP32((LSA_UINT32)pChUsr->Slave.AdjustTermP);		
	pDiagBlock->ClpTermIHigh.Dword = GSY_SWAP32((LSA_UINT32)((LSA_UINT64)(pChUsr->Slave.AdjustTermI) >> 32));
	pDiagBlock->ClpTermILow.Dword = GSY_SWAP32((LSA_UINT32)pChUsr->Slave.AdjustTermI);		
	pDiagBlock->MasterSeconds.Dword = GSY_SWAP32(pDiag->MasterSeconds);		
	pDiagBlock->MasterNanoseconds.Dword = GSY_SWAP32(pDiag->MasterNanoseconds);
	pDiagBlock->ExtPllOffset.Dword = GSY_SWAP32((LSA_UINT32)ExtPllOffset);
	pDiagBlock->LocalOffset.Dword = GSY_SWAP32((LSA_UINT32)(pDiag->Offset));
	pDiagBlock->AdjustInterval.Dword = GSY_SWAP32((LSA_UINT32)(pDiag->AdjustInterval));
	pDiagBlock->DriftInterval.Dword = GSY_SWAP32((LSA_UINT32)(pDiag->DriftInterval));
	pDiagBlock->SyncDelay.Dword = GSY_SWAP32(SyncDelay);			
	pDiagBlock->RecvDelay.Dword = GSY_SWAP32(RecvDelay);			
	pDiagBlock->PortId.Word = GSY_SWAP16(PortId);				
	pDiagBlock->MasterMacAddr = pDiag->MasterMacAddr;		
	pDiagBlock->SyncId = SyncId;				
	pDiagBlock->ComClass = ComClass;	  		
	pDiagBlock->SlaveState = pChUsr->Slave.State;	  		
	pDiagBlock->DiagSource = (LSA_UINT8)CallId;  		
	pDiagBlock->BlockNo.Word = GSY_SWAP16(pChUsr->DiagNo);			
	pDiagBlock->LocalTimeHigh.Dword = GSY_SWAP32(SetTimeHigh);			
	pDiagBlock->LocalTimeLow.Dword = GSY_SWAP32(SetTimeLow);			
	pDiagBlock->TimeStamp.Dword = GSY_SWAP32(TimeStamp);			
	if (GSY_SLAVE_STATE_OFF != pChUsr->Slave.State)
		pDiagBlock->SeqId.Word = GSY_SWAP16(pChUsr->Slave.Act.SeqId);
	else			
		pDiagBlock->SeqId.Word = GSY_SWAP16(pChUsr->Master.SeqId);

	/* Increment diagnosis block counter and index
	 * Old entries above counter GSY_DIAG_BLOCK_COUNT will be overwritten
	*/
	pChUsr->DiagNo++;
	if (pChUsr->DiagCount < GSY_DIAG_BLOCK_COUNT)
		pChUsr->DiagCount++;

	if (++pChUsr->DiagIdxWr	>= GSY_DIAG_BLOCK_COUNT)
		pChUsr->DiagIdxWr = 0;

	GSY_DIAG_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_PrmDiagWrite(%x:%u) DiagIdxWr=%02u DiagNo=%05u",
					pChUsr, SyncId, pChUsr->DiagIdxWr, pChUsr->DiagNo);

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_PrmDiagWrite() pDiag=%X Port=%d ComClass=%d",
					pDiag, PortId, ComClass);
}

/*****************************************************************************/
/* Internal function: gsy_PrmUserInd()                                       */
/* Copy PRM event(s) to upper RQB                                            */
/*****************************************************************************/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_PrmUserInd(
GSY_CH_USR_PTR			pChUsr,
GSY_UPPER_RQB_PTR_TYPE  pRbu)
{
	LSA_UINT16	RespVal = GSY_RSP_OK_ACTIVE;
	GSY_UPPER_PRM_IND_PTR_TYPE	pDiag;

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_PrmUserInd() pChUsr=%X Event=%d OldEvent=%d",
					pChUsr, pChUsr->Event.SyncEvent, pChUsr->Event.OldEvent);

	GSY_PRM_TRACE_07(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PrmUserInd(%02x) Port=%u ExtChaErrType/Prop=0x%04x/0x%04x, NewOldPrm=%u NewPrm=%u QueueEventCount=%u",
					pChUsr->SyncId, pChUsr->Event.PortId, pChUsr->Event.Prm.ExtChannelErrorType, pChUsr->Event.Prm.ChannelProperties, pChUsr->Event.NewOldPrm, pChUsr->Event.NewPrm, pChUsr->QueueEvent.Count);

	if (pChUsr->Event.NewPrm)
	{
		/* Fill PRM Indication RQB
		*/
		GSY_UPPER_RQB_PRMIND_PORTID_SET(pRbu, pChUsr->Event.PortId);
		if (GSY_PRM_EVENT_NONE == pChUsr->Event.Prm.ExtChannelErrorType)
		{
			/* First Event is SYNC_OK = no error -> empty Indication
			*/
			GSY_UPPER_RQB_PRMIND_ACTCNT_SET(pRbu, 0);
		}
		else
		{
			GSY_UPPER_RQB_PRMIND_ACTCNT_SET(pRbu, 1);
			pDiag = GSY_UPPER_RQB_PRMIND_PTR_GET(pRbu);
			if (pChUsr->Event.NewOldPrm)
			{
				/* More than one event: first copy old one
				*/
				GSY_UPPER_RQB_PRMIND_ACTCNT_SET(pRbu, 2);
				GSY_COPY_LOCAL_TO_UPPER_MEM(&pChUsr->Event.OldPrm, pDiag,
											(LSA_UINT16)sizeof(GSY_PRM_IND_DATA_TYPE), pChUsr->pSys);
				pDiag++;
				pChUsr->Event.NewOldPrm = LSA_FALSE;

				/* 260207lrg001: Store indicated old PRM Event
				*/
				if (GSY_PRM_EVENT_NO_MESSAGE_RECEIVED == pChUsr->Event.OldPrm.ExtChannelErrorType)
				{
					if (GSY_PRM_EVENT_APPEARS & pChUsr->Event.OldPrm.ChannelProperties)
						pChUsr->Event.IndFlags |= GSY_PRM_IND_FLAGS_TIMEOUT;
					else
						pChUsr->Event.IndFlags &= ~GSY_PRM_IND_FLAGS_TIMEOUT;
				}
				else
				{
					if (GSY_PRM_EVENT_APPEARS & pChUsr->Event.OldPrm.ChannelProperties)
						pChUsr->Event.IndFlags |= GSY_PRM_IND_FLAGS_NOTSYNC;
					else
						pChUsr->Event.IndFlags &= ~GSY_PRM_IND_FLAGS_NOTSYNC;
				}
				GSY_PRM_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PrmUserInd(%02x) OldExtChaErrType/Prop=0x%04x/0x%04x",
					pChUsr->SyncId, pChUsr->Event.OldPrm.ExtChannelErrorType, pChUsr->Event.OldPrm.ChannelProperties);
			}
			GSY_COPY_LOCAL_TO_UPPER_MEM(&pChUsr->Event.Prm, pDiag,
										(LSA_UINT16)sizeof(GSY_PRM_IND_DATA_TYPE), pChUsr->pSys);

			/* Store indicated PRM Event
			*/
			if (GSY_PRM_EVENT_NO_MESSAGE_RECEIVED == pChUsr->Event.Prm.ExtChannelErrorType)
			{
				if (GSY_PRM_EVENT_APPEARS & pChUsr->Event.Prm.ChannelProperties)
					pChUsr->Event.IndFlags |= GSY_PRM_IND_FLAGS_TIMEOUT;
				else
					pChUsr->Event.IndFlags &= ~GSY_PRM_IND_FLAGS_TIMEOUT;
			}
			else
			{
				if (GSY_PRM_EVENT_APPEARS & pChUsr->Event.Prm.ChannelProperties)
					pChUsr->Event.IndFlags |= GSY_PRM_IND_FLAGS_NOTSYNC;
				else
					pChUsr->Event.IndFlags &= ~GSY_PRM_IND_FLAGS_NOTSYNC;
			}
		}
		RespVal = GSY_RSP_OK;
		pChUsr->Event.NewPrm = LSA_FALSE;
	}

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_PrmUserInd() pRbu=%X EventSeconds=%u EventNanos=%u",
					pRbu, pChUsr->Event.Prm.Seconds, pChUsr->Event.Prm.Nanoseconds);
	return(RespVal);
}

/*****************************************************************************/
/* Internal function: gsy_PrmRead()                                          */
/* Read record                                                               */
/*****************************************************************************/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_PrmRead(
GSY_CH_USR_PTR			pChUsr,
GSY_UPPER_RQB_PTR_TYPE  pRbu)
{
	GSY_UPPER_SYNC_DATA_PTR	pSyncData;
	GSY_UPPER_BYTE_PTR_TYPE	pPortData;
	GSY_UPPER_BYTE_PTR_TYPE	pDiagUpper;
	GSY_DIAG_DATA_BLOCK		*pDiagRd;
	LSA_INT		IdxRd;
	LSA_UINT32	MaxCnt, ActCnt;
	LSA_UINT16	BlockLen;
	LSA_UINT16	PortIdx, PortId = GSY_UPPER_RQB_PRMRECORD_PORT_GET(pRbu);
	LSA_UINT16	RespVal = GSY_RSP_OK;
	LSA_UINT32	RecordIndex = GSY_UPPER_RQB_PRMRECORD_IDX_GET(pRbu);
	LSA_UINT32	RecordLen = GSY_UPPER_RQB_PRMRECORD_LEN_GET(pRbu);

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_PrmRead() pChUsr=%X RecordIndex=0x%x SyncId=0x%02x",
					pChUsr, RecordIndex, pChUsr->SyncId);

	GSY_UPPER_RQB_PRMRECORD_LEN_SET(pRbu, 0);

 	if (((GSY_PORT_DATA_IDX == RecordIndex) && (0 == PortId))
	||  ((GSY_PORT_DATA_IDX != RecordIndex) && (0 != PortId)))
	{
		GSY_ERROR_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmRead() Invalid record PortId=%u",
				PortId);
		RespVal = GSY_RSP_ERR_PRM_PORTID;
	}
	else
	{
		if (LSA_HOST_PTR_ARE_EQUAL(GSY_UPPER_RQB_PRMRECORD_PTR_GET(pRbu), LSA_NULL))
		{
			GSY_ERROR_TRACE_00(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmRead() NULL data pointer");
			RespVal = GSY_RSP_ERR_PARAM;
		}
		/* If record index is valid...
		*/
	  	else if (GSY_DIAG_DATA_IDX == RecordIndex)
		{
			/* Read diagnosis record
			*/
			RespVal = GSY_RSP_ERR_PRM_DATA;
			if (GSY_DIAG_BLOCK_LEN <= RecordLen)
			{
				/* Calculate count of entries to read
				*/
				MaxCnt = RecordLen / GSY_DIAG_BLOCK_LEN;
				if (MaxCnt > pChUsr->DiagCount)
					MaxCnt = pChUsr->DiagCount;

				/* Find read index:
				 * always read the newest [MaxCnt] entries
				*/
				IdxRd = (LSA_INT32)(pChUsr->DiagIdxWr - MaxCnt);
				if (IdxRd < 0)
					IdxRd += GSY_DIAG_BLOCK_COUNT;		/* WrapAround */

				GSY_DIAG_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"  - gsy_PrmRead(%x:%u) IdxRd=%02u MaxCnt=%02u",
							pChUsr, pChUsr->SyncId, IdxRd, MaxCnt);

				pDiagUpper = GSY_UPPER_RQB_PRMRECORD_PTR_GET(pRbu);
				for (ActCnt = 0; ActCnt < MaxCnt; ActCnt++)
				{
					/* Copy one diagnosis block into user buffer
					*/
					pDiagRd = &pChUsr->DiagRecord.DiagBlock[IdxRd];
					GSY_COPY_LOCAL_TO_UPPER_MEM(pDiagRd, pDiagUpper, GSY_DIAG_BLOCK_LEN, pChUsr->pSys);
					pDiagUpper += GSY_DIAG_BLOCK_LEN;
					if (++IdxRd >= GSY_DIAG_BLOCK_COUNT)
						IdxRd = 0;						/* WrapAround */
				}
				GSY_UPPER_RQB_PRMRECORD_LEN_SET(pRbu, MaxCnt * GSY_DIAG_BLOCK_LEN);
				RespVal = GSY_RSP_OK;
			}
		}
		else if ((GSY_SYNC_DATA_IDX == RecordIndex) || (GSY_SYNC_TIME_IDX == RecordIndex))	//Task 1311090
		{
			/* Record index is valid for clock or time sync record: check active SyncId
			*/
			if ((((GSY_SYNCID_CLOCK == pChUsr->SyncId) && (GSY_SYNC_DATA_IDX == RecordIndex)) 
			||  ((GSY_SYNCID_TIME == pChUsr->SyncId)  && (GSY_SYNC_TIME_IDX == RecordIndex)))
/*			&&   (  (GSY_SLAVE_STATE_OFF != pChUsr->Slave.State) 
                 || (GSY_MASTER_STATE_OFF != pChUsr->Master.State) 
                 || (GSY_MASTER_CONTROL_STATE_NONE != pChUsr->MasterControlState)
                 )*/
               )
			{
				/* If a Sync record for Master or Slave is active...
				*/
				RespVal = GSY_RSP_ERR_PRM_DATA;
				BlockLen = GSY_SWAP16(pChUsr->SyncData.BlockLength.Word) + GSY_BLOCK_TYPE_LEN_SIZE;
				if (BlockLen <= RecordLen)
				{
					/* Copy PDSyncData record into user buffer
					*/
					pSyncData = (GSY_UPPER_SYNC_DATA_PTR)(void*)GSY_UPPER_RQB_PRMRECORD_PTR_GET(pRbu);
					GSY_COPY_LOCAL_TO_UPPER_MEM(&pChUsr->SyncData, pSyncData, BlockLen, pChUsr->pSys);
					GSY_UPPER_RQB_PRMRECORD_LEN_SET(pRbu, BlockLen);
					RespVal = GSY_RSP_OK;
				}
				else
					GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmRead() Invalid PDSyncData record length=%u BlockLen=%u",
						RecordLen, BlockLen);
			}
		}
		else if ((GSY_SYNCID_CLOCK == pChUsr->SyncId) && (GSY_SYNC_PLL_IDX == RecordIndex)) 
		{
			/* Record index is valid for PDSyncPLL record: Parameter check
			*/
			if (pChUsr->SyncPLLActive)
			{
				/* If a PLL record for Master is active...
				*/
				RespVal = GSY_RSP_ERR_PRM_DATA;
				BlockLen = GSY_SWAP16(pChUsr->SyncPLL.BlockLength.Word) + GSY_BLOCK_TYPE_LEN_SIZE;
				if (BlockLen <= RecordLen)
				{
					/* Copy PDSyncPLL record into user buffer
					*/
					GSY_UPPER_SYNC_PLL_PTR pSyncPLL = (GSY_UPPER_SYNC_PLL_PTR)(void*)GSY_UPPER_RQB_PRMRECORD_PTR_GET(pRbu);
					GSY_COPY_LOCAL_TO_UPPER_MEM(&pChUsr->SyncPLL, pSyncPLL, BlockLen, pChUsr->pSys);
					GSY_UPPER_RQB_PRMRECORD_LEN_SET(pRbu, BlockLen);
					RespVal = GSY_RSP_OK;
				}
				else
					GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmRead() Invalid PDSyncPLL record length=%u BlockLen=%u",
						RecordLen, BlockLen);
			}
		}
	  	else if ((GSY_PORT_DATA_IDX == RecordIndex) && (0 != PortId))
		{
			/* Read PDPortDataAdjust record
			*/
			PortIdx = PortId - 1;
			if ((GSY_PRM_PORT_STATE_CFG == pChUsr->pChSys->Port[PortIdx].PrmDomainState)
			||  (GSY_PRM_PORT_STATE_CFG == pChUsr->pChSys->Port[PortIdx].PrmPeerState))
			{
				BlockLen = GSY_SWAP16(pChUsr->pChSys->Port[PortIdx].PrmPortDataAdjust.BlockLength.Word) + GSY_BLOCK_TYPE_LEN_SIZE;
				if (BlockLen <= RecordLen)
				{
					/* Copy PortDataAdjust header into user buffer
					*/
					GSY_UPPER_RQB_PRMRECORD_LEN_SET(pRbu, BlockLen);
					pPortData = (GSY_UPPER_BYTE_PTR_TYPE)GSY_UPPER_RQB_PRMRECORD_PTR_GET(pRbu);
					GSY_COPY_LOCAL_TO_UPPER_MEM(&pChUsr->pChSys->Port[PortIdx].PrmPortDataAdjust, pPortData, GSY_PORT_DATA_HDR_LEN, pChUsr->pSys);
					pPortData += GSY_PORT_DATA_HDR_LEN;
					if (GSY_PRM_PORT_STATE_CFG == pChUsr->pChSys->Port[PortIdx].PrmDomainState)
					{
						/* Copy DomainBoundary Subblock
						*/
						BlockLen = GSY_SWAP16(pChUsr->pChSys->Port[PortIdx].DomainBoundary.BlockLength.Word) + GSY_BLOCK_TYPE_LEN_SIZE;
						GSY_COPY_LOCAL_TO_UPPER_MEM(&pChUsr->pChSys->Port[PortIdx].DomainBoundary, pPortData, BlockLen, pChUsr->pSys);
						pPortData += BlockLen;
					}
					if (GSY_PRM_PORT_STATE_CFG == pChUsr->pChSys->Port[PortIdx].PrmPeerState)
					{
						/* Copy PeerToPeerBoundary Subblock
						*/
						BlockLen = GSY_SWAP16(pChUsr->pChSys->Port[PortIdx].PeerBoundary.BlockLength.Word) + GSY_BLOCK_TYPE_LEN_SIZE;
						GSY_COPY_LOCAL_TO_UPPER_MEM(&pChUsr->pChSys->Port[PortIdx].PeerBoundary, pPortData, BlockLen, pChUsr->pSys);
					}
					RespVal = GSY_RSP_OK;
				}
				else
				{
					GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmRead() Invalid PDPortDataAdjust record length: %d BlockLen=%u",
						RecordLen, BlockLen);
					RespVal = GSY_RSP_ERR_PRM_DATA;
				}
			}
		}
		else
		{
			GSY_ERROR_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmRead() Invalid record index: 0x%x",
					RecordIndex);
			RespVal = GSY_RSP_ERR_PRM_INDEX;
		}
	}

	GSY_PRM_TRACE_06(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PrmRead(%02x) Port=%u RecordIndex=0x%08x, RecordLen=%u/%u RespVal=0x%04x",
					pChUsr->SyncId, PortId, RecordIndex, RecordLen, GSY_UPPER_RQB_PRMRECORD_LEN_GET(pRbu), RespVal);

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_PrmRead() pChUsr=%X RecordLen=%u RespVal=0x%x",
					pChUsr, RecordLen, RespVal);
	return(RespVal);
}

/*****************************************************************************/
/* Internal function: gsy_PrmPrepare()                                       */
/* Load default values for synchronisation in temporary (B) records          */
/*****************************************************************************/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_PrmPrepare(
GSY_CH_USR_PTR			pChUsr,
GSY_UPPER_RQB_PTR_TYPE  pRbu)
{
	LSA_UINT16	PortId, RespVal = GSY_RSP_OK;

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_PrmPrepare() pChUsr=%X pRbu=%X SyncId=0x%02x",
					pChUsr, pRbu, pChUsr->SyncId);

	pChUsr->PrmState = GSY_PRM_STATE_PREPARE;

	/* Load default record (all 0 => SyncStop) into PrmData
	*/
	GSY_MEMSET_LOCAL(&pChUsr->PrmData, 0, sizeof(GSY_SYNC_DATA_RECORD));
	pChUsr->PrmData.BlockLength.Word = GSY_SWAP16(GSY_SYNC_DATA_BLOCKLEN);
	pChUsr->PrmData.BlockType.Word = GSY_SWAP16(GSY_SYNC_DATA_BLOCKTYPE);
	pChUsr->PrmData.SyncProperties.Word = GSY_SWAP16((GSY_SYNCID_NONE << 8) | GSY_SYNC_ROLE_LOCAL);
	pChUsr->PrmRecordIndexWritten = 0;
	pChUsr->PrmClockAndTimeWritten = LSA_FALSE;

	for (PortId = 0; PortId < pChUsr->pChSys->PortCount; PortId++)
	{
		pChUsr->pChSys->Port[PortId].PrmDomainWritten = LSA_FALSE;
		pChUsr->pChSys->Port[PortId].PrmPeerWritten = LSA_FALSE;

		/* 161106js001	Flag setting default values if no record is loaded
		*/
		pChUsr->pChSys->Port[PortId].PrmDomainCfg = LSA_TRUE;
	}

	/* 051206lrg001: Store default SendClockFactor
	*/
	pChUsr->PrmSendClock.Word = GSY_SWAP16(GSY_SEND_CLOCK_DEFAULT);

	/* Load default record (all 0 => no PLL sync) into PrmPLL
	*/
	GSY_MEMSET_LOCAL(&pChUsr->PrmPLL, 0, sizeof(GSY_SYNC_PLL_RECORD));
	pChUsr->PrmPLL.BlockLength.Word = GSY_SWAP16(GSY_SYNC_PLL_BLOCKLENGTH);
	pChUsr->PrmPLL.BlockType.Word = GSY_SWAP16(GSY_SYNC_PLL_BLOCKTYPE);
	pChUsr->PrmPLLWritten = LSA_FALSE;

	GSY_PRM_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PrmPrepare(%02x) RespVal=0x%x", pChUsr->SyncId, RespVal);

	GSY_FUNCTION_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_PrmPrepare() pChUsr=%X",
					pChUsr);

	LSA_UNUSED_ARG(pRbu);

	return(RespVal);
}

/*****************************************************************************/
/* Internal function: gsy_PrmWritePortData()                                 */
/* Load DomainBoundaries for Forwarding configuration and PeerToPeer-        */
/* Boundaries for Delay measurement configuration in temporary (B) Record    */
/*****************************************************************************/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_PrmWritePortData(
GSY_CH_USR_PTR			pChUsr,
GSY_UPPER_RQB_PTR_TYPE  pRbu)
{
	GSY_UPPER_SUBBLOCK_PTR	pSubBlock;
	LSA_UINT8 				*pByte;
	GSY_CH_PORT				*pPort;
	GSY_UPPER_PORT_DATA_PTR pPortData = (GSY_UPPER_PORT_DATA_PTR)(void*)GSY_UPPER_RQB_PRMRECORD_PTR_GET(pRbu);
	LSA_UINT16				PortId = GSY_UPPER_RQB_PRMRECORD_PORT_GET(pRbu);
	LSA_UINT32	Offset0, Offset1, ErrOffset = GSY_PRM_ERR_OFFSET_DEFAULT;
	LSA_UINT32	RecordLen, RecordIdx;
	LSA_UINT32	DomainBoundary, DomainBoundaryIngress = 0;
	LSA_UINT16	BlockType, BlockLen, ActLen, Properties, Padding1, Padding2, Slot, Subslot, CheckLen;
	LSA_UINT8	VersionLow;
	LSA_UINT16	RespVal = GSY_RSP_ERR_PRM_PORTID;
	LSA_BOOL	HeaderWritten = LSA_FALSE;

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_PrmWritePortData() pChUsr=%X Port=%d SyncId=0x%02x",
					pChUsr, PortId, pChUsr->SyncId);

	/* Check port ID
	*/
	if ((0 == PortId)
	|| (GSY_CH_MAX_PORTS < PortId))		// TODO gl:
	{
		GSY_ERROR_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWritePortData() Invalid port ID=%u",
				PortId);
	}
	else
	{
		/* Check record length
		*/
		pPort = &pChUsr->pChSys->Port[PortId-1];
		RespVal = GSY_RSP_ERR_PRM_DATA;
		RecordLen = GSY_UPPER_RQB_PRMRECORD_LEN_GET(pRbu);

		if ((RecordLen < GSY_BLOCK_TYPE_LEN_SIZE)
		||	(RecordLen != (LSA_UINT32)(GSY_BLOCK_TYPE_LEN_SIZE + GSY_SWAP16(pPortData->Header.BlockLength.Word))))
		{
			GSY_ERROR_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWritePortData() Invalid record length=%u",
					RecordLen);
		}
		else
		{
			RespVal = GSY_RSP_ERR_PRM_BLOCK;

			/* Check block type and length
			*/
			BlockType = GSY_SWAP16(pPortData->Header.BlockType.Word);
			BlockLen = GSY_SWAP16(pPortData->Header.BlockLength.Word);
			if (BlockType != GSY_PORT_DATA_BLOCKTYPE)
			{
				ErrOffset = GSY_OFFSET_GET(pPortData, &pPortData->Header.BlockType);
				GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWritePortData() Invalid BlockType=0x%04x ErrOffset=%u",
						BlockType, ErrOffset);
			}
			else if ((GSY_BLOCK_TYPE_LEN_SIZE + BlockLen) < GSY_PORT_DATA_HDR_LEN)
			{
				ErrOffset = GSY_OFFSET_GET(pPortData, &pPortData->Header.BlockLength);
				GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWritePortData() Invalid BlockLength=%u ErrOffset=%u",
						BlockLen, ErrOffset);
			}
			else
			{
				/* Check block slot and subslot
				*/
				Slot = GSY_UPPER_RQB_PRMRECORD_SLOT_GET(pRbu);
				Subslot = GSY_UPPER_RQB_PRMRECORD_SUBSLOT_GET(pRbu);
				Padding1 = GSY_SWAP16(pPortData->Header.Padding1.Word);
	
				if (Padding1 != 0)
				{
					ErrOffset = GSY_OFFSET_GET(pPortData, &pPortData->Header.Padding1);
					GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWritePortData() Invalid padding=%u ErrOffset=%u",
							Padding1, ErrOffset);
				}
				else if (Slot != GSY_SWAP16(pPortData->Header.SlotNumber.Word))
				{
					ErrOffset = GSY_OFFSET_GET(pPortData, &pPortData->Header.SlotNumber);
					GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWritePortData() Invalid RQB-slot=%u ErrOffset=%u",
							Slot, ErrOffset);
				}
				else if (Subslot != GSY_SWAP16(pPortData->Header.SubslotNumber.Word))
				{
					ErrOffset = GSY_OFFSET_GET(pPortData, &pPortData->Header.SubslotNumber);
					GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWritePortData() Invalid RQB-subslot=%u ErrOffset=%u",
							Subslot, ErrOffset);
				}
				else
				{
					/* 161106js001: if no DomainBoundary record present 
					 * -> use default values. No error
					 * An prior written record becomes invalid
					*/
					RespVal = GSY_RSP_OK;
					pPort->PrmDomainWritten = LSA_FALSE;
					pPort->PrmPeerWritten = LSA_FALSE;
	
					/* Find AdjustDomainBoundary or PeerToPeerBoundary Subblock
					*/
					pByte = (LSA_UINT8 *)&pPortData->SubBlock;
					RecordIdx = GSY_PORT_DATA_HDR_LEN;
	
					/* 161106js001: access BlockType only if Type/Len/Version of SubBlock is inside record
					*/
					while ((RespVal == GSY_RSP_OK) 
					&& 	   ((RecordIdx + GSY_BLOCK_HEADER_SIZE) < RecordLen))
					{
						pSubBlock = (GSY_UPPER_SUBBLOCK_PTR)pByte;
						BlockType = GSY_SWAP16(pSubBlock->BlockType.Word);
						BlockLen = GSY_BLOCK_TYPE_LEN_SIZE + GSY_SWAP16(pSubBlock->BlockLength.Word);
						VersionLow = pSubBlock->VersionLow;
	
						GSY_PRM_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"--- gsy_PrmWritePortData() Subblock=0x%04x Length=%u VersionLow=%u",
								BlockType, BlockLen, VersionLow);
#ifdef GSY_MESSAGE
						/* 161106js001: TBD: check record consitency. Length has to match
						 *              TBD: DomainBoundary must not be located at arbitrary position
						 *                   and only one subblock is allowed in record
						*/
#endif /* GSY_MESSAGE */
	
						if (GSY_DOMAIN_BOUNDARY_BLOCKTYPE == BlockType) 
						{
							/* Check rest length and block content of DomainBoundary subblock
							*/
							if (pChUsr->pChSys->RxTxNanos == 0)
							{
								/* 170107lrg001: no DomainBoundary Subblock is allowed for EDD without timestamps
								*/
								CheckLen = 0;
								RespVal = GSY_RSP_ERR_PRM_BLOCK;
								ErrOffset = GSY_OFFSET_GET(pPortData, &pSubBlock->BlockType);
								GSY_ERROR_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWritePortData() No HW support for subblock DomainBoundary ErrOffset=%u",
										ErrOffset);
							}
							else if (VersionLow > 1)
							{
								CheckLen = 0;
								RespVal = GSY_RSP_ERR_PRM_BLOCK;
								ErrOffset = GSY_OFFSET_GET(pPortData, &pSubBlock->VersionLow);
								GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWritePortData() DomainBoundary: Invalid VersionLow=%u ErrOffset=%u",
										VersionLow, ErrOffset);
							}
							else if (VersionLow == 1)
							{
								CheckLen = GSY_SUBBLOCK_HEADER_SIZE + sizeof(GSY_DOMAIN_BOUNDARY1);
							}
							else
							{
								CheckLen = GSY_SUBBLOCK_HEADER_SIZE + sizeof(GSY_DOMAIN_BOUNDARY);
							}
							if ((CheckLen != 0)
							&&  (BlockLen <= (RecordLen - RecordIdx)) && (BlockLen == CheckLen))	/* 161106js001: length must match ! */
							{
								Padding1 = GSY_SWAP16(pSubBlock->Padding1.Word);
								if (VersionLow == 1)
								{
									Padding2 = GSY_SWAP16(pSubBlock->Boundary.Domain1.Padding2.Word);
									Properties = GSY_SWAP16(pSubBlock->Boundary.Domain1.Properties.Word);
									Offset0 = GSY_OFFSET_GET(pPortData, &pSubBlock->Boundary.Domain1.Padding2);
									Offset1 = GSY_OFFSET_GET(pPortData, &pSubBlock->Boundary.Domain1.Properties);
								}
								else
								{
									Padding2 = GSY_SWAP16(pSubBlock->Boundary.Domain.Padding2.Word);
									Properties = GSY_SWAP16(pSubBlock->Boundary.Domain.Properties.Word);
									Offset0 = GSY_OFFSET_GET(pPortData, &pSubBlock->Boundary.Domain.Padding2);
									Offset1 = GSY_OFFSET_GET(pPortData, &pSubBlock->Boundary.Domain.Properties);
								}
								if (0 != Padding1)
								{
									RespVal = GSY_RSP_ERR_PRM_BLOCK;
									ErrOffset = GSY_OFFSET_GET(pPortData, &pSubBlock->Padding1);
									GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWritePortData() DomainBoundary: Invalid Padding1=%u ErrOffset=%u",
											Padding1, ErrOffset);
								}
								else if (0 != Padding2)
								{
									RespVal = GSY_RSP_ERR_PRM_BLOCK;
									ErrOffset = Offset0;
									GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWritePortData() DomainBoundary: Invalid Padding2=%u ErrOffset=%u",
											Padding2, ErrOffset);
								}
								else if (0 != Properties)
								{
									RespVal = GSY_RSP_ERR_PRM_BLOCK;
									ErrOffset = Offset1;
									GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWritePortData() DomainBoundary: Invalid Properties=0x%04x ErrOffset=%u",
											Properties, ErrOffset);
								}
								/* 300310lrg001: check Boundaries for supported SyncIds
								*/
								if (VersionLow == 1)
								{
									DomainBoundaryIngress = GSY_SWAP32(pPort->PrmDomainBoundary.Boundary.Domain1.DomainBoundaryIngress.Dword);
									DomainBoundary = GSY_SWAP32(pPort->PrmDomainBoundary.Boundary.Domain1.DomainBoundaryEgress.Dword);
									Offset0 = GSY_OFFSET_GET(pPortData, &pSubBlock->Boundary.Domain1.DomainBoundaryEgress);
								}
								else
								{
									DomainBoundary = GSY_SWAP32(pPort->PrmDomainBoundary.Boundary.Domain.DomainBoundary.Dword);
									Offset0 = GSY_OFFSET_GET(pPortData, &pSubBlock->Boundary.Domain.DomainBoundary);
								}
								if (DomainBoundary > GSY_SYNC_ID_SUPPORTED)
								{
									RespVal = GSY_RSP_ERR_PRM_BLOCK;
									ErrOffset = Offset0;
									GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWritePortData() DomainBoundary=0x%08x > GSY_SYNC_ID_SUPPORTED=0x%08x ErrOffset=%u",
												DomainBoundary, GSY_SYNC_ID_SUPPORTED, ErrOffset);
								}
								else if ((VersionLow == 1) && (DomainBoundaryIngress > GSY_SYNC_ID_SUPPORTED))
								{
									RespVal = GSY_RSP_ERR_PRM_BLOCK;
									ErrOffset = GSY_OFFSET_GET(pPortData, &pSubBlock->Boundary.Domain1.DomainBoundaryIngress);
									GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWritePortData() DomainBoundaryIngress=0x%08x  > GSY_SYNC_ID_SUPPORTED=0x%08x ErrOffset=%u",
												DomainBoundaryIngress, GSY_SYNC_ID_SUPPORTED, ErrOffset);
								}
								else
								{
									/* Copy PortDataAdjust header and DomainBoundary Subblock into Port data and update PrmState
									*/
									if (!HeaderWritten)
									{
										pPort->PrmPortDataAdjust = pPortData->Header;
										ActLen = GSY_PORT_DATA_HDR_LEN - GSY_BLOCK_TYPE_LEN_SIZE;
										HeaderWritten = LSA_TRUE;
									}
									else
									{
										ActLen = GSY_SWAP16(pPort->PrmPortDataAdjust.BlockLength.Word);
									}
									pPort->PrmPortDataAdjust.BlockLength.Word = GSY_SWAP16(ActLen + BlockLen);
									pPort->PrmDomainBoundary = *pSubBlock;
									pPort->PrmDomainWritten = LSA_TRUE;
									pChUsr->PrmState = GSY_PRM_STATE_WRITE;
									RespVal = GSY_RSP_OK;
	
									if (VersionLow == 1)
									{
										GSY_PRM_TRACE_05(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PrmWritePortData(%02x) ActLen=%u Port=%u DomainBoundaryIngress=0x%08x DomainBoundaryEgress=0x%08x",
											pChUsr->SyncId, ActLen, PortId, 
											GSY_SWAP32(pPort->PrmDomainBoundary.Boundary.Domain1.DomainBoundaryIngress.Dword),
											GSY_SWAP32(pPort->PrmDomainBoundary.Boundary.Domain1.DomainBoundaryEgress.Dword));
									}
									else
									{
										GSY_PRM_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PrmWritePortData(%02x) ActLen=%u Port=%u DomainBoundary=0x%08x",
											pChUsr->SyncId, ActLen, PortId, 
											GSY_SWAP32(pPort->PrmDomainBoundary.Boundary.Domain.DomainBoundary.Dword));
									}
								}
							}
							else if (CheckLen != 0)
							{
								/* 161106js001: Error Trace
								*/
								RespVal = GSY_RSP_ERR_PRM_BLOCK;
								ErrOffset = GSY_OFFSET_GET(pPortData, &pSubBlock->BlockLength);
								GSY_ERROR_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWritePortData() DomainBoundary subblock length mismatch: BlockLen/RecordLen/RecordIdx=%u/%u/%u ErrOffset=%u",
										BlockLen, RecordLen, RecordIdx, ErrOffset);
							}
						}
						else if (GSY_PEER_BOUNDARY_BLOCKTYPE == BlockType) 
						{
							/* Check rest length and block content of PeerToPeerBoundary subblock
							*/
							if ((BlockLen > (RecordLen - RecordIdx))
							||  (BlockLen != (GSY_SUBBLOCK_HEADER_SIZE + sizeof(GSY_PEER_BOUNDARY))))
							{
								/* 161106js001: Error Trace
								*/
								RespVal = GSY_RSP_ERR_PRM_BLOCK;
								ErrOffset = GSY_OFFSET_GET(pPortData, &pSubBlock->BlockLength);
								GSY_ERROR_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWritePortData() PeerToPeerBoundary subblock length mismatch: BlockLen/RecordLen/RecordIdx=%u/%u/%u ErrOffset=%u",
										BlockLen, RecordLen, RecordIdx, ErrOffset);
							}
							else
							{
								Padding1 = GSY_SWAP16(pSubBlock->Padding1.Word);
								Padding2 = GSY_SWAP16(pSubBlock->Boundary.Peer.Padding2.Word);
								Properties = GSY_SWAP16(pSubBlock->Boundary.Peer.Properties.Word);
								if (0 != Padding1)
								{
									RespVal = GSY_RSP_ERR_PRM_BLOCK;
									ErrOffset = GSY_OFFSET_GET(pPortData, &pSubBlock->Padding1);
									GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWritePortData() PeerToPeerBoundary: Invalid Padding1=%u ErrOffset=%u",
											Padding1, ErrOffset);
								}
								else if (0 != Padding2)
								{
									RespVal = GSY_RSP_ERR_PRM_BLOCK;
									ErrOffset = GSY_OFFSET_GET(pPortData, &pSubBlock->Boundary.Peer.Padding2);
									GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWritePortData() PeerToPeerBoundary: Invalid Padding2=%u ErrOffset=%u",
											Padding2, ErrOffset);
								}
								else if (0 != Properties)
								{
									RespVal = GSY_RSP_ERR_PRM_BLOCK;
									ErrOffset = GSY_OFFSET_GET(pPortData, &pSubBlock->Boundary.Peer.Properties);
									GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWritePortData() PeerToPeerBoundary: Invalid Properties=%u ErrOffset=%u",
											Properties, ErrOffset);
								}
								/* 190608lrg001: Reject record on EDDS only if PTCP boundary (Bit 1) is set
								*/
								else if ((pChUsr->pChSys->RxTxNanos == 0)
									  &&  ((GSY_SWAP32(pSubBlock->Boundary.Peer.PeerBoundary.Dword) & 2) == 2))
								{
									/* No PeerToPeerBoundary Subblock with PTCP boundary is allowed for EDD without timestamp (EDDS)
									*/
									RespVal = GSY_RSP_ERR_PRM_BLOCK;
									ErrOffset = GSY_OFFSET_GET(pPortData, &pSubBlock->Boundary.Peer.PeerBoundary);
									GSY_ERROR_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWritePortData() No HW support for subblock PeerToPeerBoundary ErrOffset=%u",
											ErrOffset);
								}
								else
								{
									/* Copy PortDataAdjust header and PeerBoundary subblock into Port data and update PrmState
									*/
									if (!HeaderWritten)
									{
										pPort->PrmPortDataAdjust = pPortData->Header;
										ActLen = GSY_PORT_DATA_HDR_LEN - GSY_BLOCK_TYPE_LEN_SIZE;
										HeaderWritten = LSA_TRUE;
									}
									else
									{
										ActLen = GSY_SWAP16(pPort->PrmPortDataAdjust.BlockLength.Word);
									}
									pPort->PrmPortDataAdjust.BlockLength.Word = GSY_SWAP16(ActLen + BlockLen);
									pPort->PrmPeerBoundary = *pSubBlock;
									pPort->PrmPeerWritten = LSA_TRUE;
									pChUsr->PrmState = GSY_PRM_STATE_WRITE;
									RespVal = GSY_RSP_OK;

									GSY_PRM_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PrmWritePortData(%02x) ActLen=%u Port=%u PeerToPeerBoundary=0x%08x",
											pChUsr->SyncId, ActLen, PortId, 
											GSY_SWAP32(pPort->PrmPeerBoundary.Boundary.Peer.PeerBoundary.Dword));
								}
							}
						}
	
						/* Next Subblock...
						*/
						RecordIdx += BlockLen;
						pByte += BlockLen;
	
					}	/* while */
	
					if (RespVal == GSY_RSP_OK)
						pChUsr->PrmState = GSY_PRM_STATE_WRITE;
				}
			}
		}
	}
	if (GSY_RSP_ERR_PRM_BLOCK == RespVal)
	{
		GSY_UPPER_RQB_PRMWRITE_ERROFFSET_SET(pRbu, ErrOffset);
	}

	GSY_PRM_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PrmWritePortData(%02x) Port=%u DomainBoundaryWritten=%u PeerBoundaryWritten=%u",
			pChUsr->SyncId, PortId, pChUsr->pChSys->Port[PortId-1].PrmDomainWritten, pChUsr->pChSys->Port[PortId-1].PrmPeerWritten);

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_PrmWritePortData() pChUsr=%X pRbu=%X RespVal=0x%x",
					pChUsr, pRbu, RespVal);
	return(RespVal);
}

/*****************************************************************************/
/* Internal function: gsy_PrmWriteSendClock()                                */
/* Store SendClockFactor for consistency check at gsy_PrmEnd()               */
/*****************************************************************************/
static LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_PrmWriteSendClock(
GSY_CH_USR_PTR			pChUsr,
GSY_UPPER_RQB_PTR_TYPE  pRbu)
{
	GSY_UPPER_SEND_CLOCK_PTR pRecordData = (GSY_UPPER_SEND_CLOCK_PTR)(void*)GSY_UPPER_RQB_PRMRECORD_PTR_GET(pRbu);
	LSA_UINT32	ErrOffset = GSY_PRM_ERR_OFFSET_DEFAULT;
	LSA_UINT32	RecordLen;
	LSA_UINT16	BlockType, BlockLen;
	LSA_UINT16	PortId = GSY_UPPER_RQB_PRMRECORD_PORT_GET(pRbu);
	LSA_UINT16	RespVal = GSY_RSP_ERR_PRM_PORTID;

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_PrmWriteSendClock() pChUsr=%X Port=%d SyncId=0x%02x",
					pChUsr, PortId, pChUsr->SyncId);

	if (0 != PortId)
	{
		GSY_ERROR_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSendClock() Invalid port ID=0x%x",
				PortId);
	}
	else
	{
		RespVal = GSY_RSP_ERR_PRM_DATA;
		RecordLen = GSY_UPPER_RQB_PRMRECORD_LEN_GET(pRbu);

		if ((RecordLen < GSY_BLOCK_TYPE_LEN_SIZE)
		||	(RecordLen != (LSA_UINT32)(GSY_BLOCK_TYPE_LEN_SIZE + GSY_SWAP16(pRecordData->BlockLength.Word))))
		{
			GSY_ERROR_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSendClock() Invalid record length=%u",
					RecordLen);
		}
		else
		{
			RespVal = GSY_RSP_ERR_PRM_BLOCK;
			BlockType = GSY_SWAP16(pRecordData->BlockType.Word);
			BlockLen = GSY_BLOCK_TYPE_LEN_SIZE + GSY_SWAP16(pRecordData->BlockLength.Word);

			if (BlockType != GSY_SEND_CLOCK_BLOCKTYPE)
			{
				ErrOffset = GSY_OFFSET_GET(pRecordData, &pRecordData->BlockType);
				GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSendClock() Invalid block type=0x%04x ErrOffset=%u",
						BlockType, ErrOffset);
			}
			else if (BlockLen < sizeof(GSY_SEND_CLOCK_RECORD))
			{
				ErrOffset = GSY_OFFSET_GET(pRecordData, &pRecordData->BlockLength);
				GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSendClock() Invalid block length=%u ErrOffset=%u",
						BlockLen, ErrOffset);
			}
			else
			{
				/* 051206lrg001: store SendClockFactor and set PrmState
				*/
				pChUsr->PrmSendClock = pRecordData->SendClockFactor;
				pChUsr->PrmState = GSY_PRM_STATE_WRITE;
				RespVal = GSY_RSP_OK;
				GSY_PRM_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PrmWriteSendClock(%02x) SendClockFactor=%u",
						pChUsr->SyncId, GSY_SWAP16(pChUsr->PrmSendClock.Word));
			}
		}
	}
	if (GSY_RSP_ERR_PRM_BLOCK == RespVal)
	{
		GSY_UPPER_RQB_PRMWRITE_ERROFFSET_SET(pRbu, ErrOffset);
	}

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_PrmWriteSendClock() pRbu=%X SendClock=%u RespVal=0x%x",
					pRbu, GSY_SWAP16(pChUsr->PrmSendClock.Word), RespVal);
	return(RespVal);
}

/*****************************************************************************/
/* Internal function: gsy_PrmWriteSyncPLL()                                  */
/* Store SyncPLL record for consistency check at gsy_PrmEnd()                */
/*****************************************************************************/
static LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_PrmWriteSyncPLL(
GSY_CH_USR_PTR			pChUsr,
GSY_UPPER_RQB_PTR_TYPE  pRbu)
{
	GSY_UPPER_SYNC_PLL_PTR	pRecordData = (GSY_UPPER_SYNC_PLL_PTR)(void*)GSY_UPPER_RQB_PRMRECORD_PTR_GET(pRbu);
	LSA_UINT16				PortId = GSY_UPPER_RQB_PRMRECORD_PORT_GET(pRbu);
	LSA_UINT32	ErrOffset = GSY_PRM_ERR_OFFSET_DEFAULT;
	LSA_UINT32	RecordLen, PLLWindow;
	LSA_UINT16	BlockType, BlockLen, ReductionRatio, PLLHWDelay, ControlInterval, SetWaitCount, Reserved;
	LSA_UINT16	RespVal = GSY_RSP_ERR_PRM_PORTID;

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_PrmWriteSyncPLL() pChUsr=%X Port=%d SyncId=0x%02x",
					pChUsr, PortId, pChUsr->SyncId);

	if (0 != PortId)	//RQ 1533977
	{
		GSY_ERROR_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSyncPLL() Invalid PortId=%u",
				PortId);
	}
	else
	{
		RespVal = GSY_RSP_ERR_PRM_DATA;
		RecordLen = GSY_UPPER_RQB_PRMRECORD_LEN_GET(pRbu);

		if ((RecordLen < GSY_BLOCK_TYPE_LEN_SIZE)
		||	(RecordLen != (LSA_UINT32)(GSY_BLOCK_TYPE_LEN_SIZE + GSY_SWAP16(pRecordData->BlockLength.Word))))
		{
			GSY_ERROR_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSyncPLL() Invalid record length=%u",
					RecordLen);
		}
		else
		{
			/* check record block
			*/
			RespVal = GSY_RSP_ERR_PRM_BLOCK;
			BlockType = GSY_SWAP16(pRecordData->BlockType.Word);
			BlockLen = GSY_SWAP16(pRecordData->BlockLength.Word);

			if (BlockType != GSY_SYNC_PLL_BLOCKTYPE)
			{
				ErrOffset = GSY_OFFSET_GET(pRecordData, &pRecordData->BlockType);
				GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSyncPLL() Invalid BlockType=0x%04x ErrOffset=%u",
						BlockType, ErrOffset);
			}
			else if (BlockLen != GSY_SYNC_PLL_BLOCKLENGTH)
			{
				ErrOffset = GSY_OFFSET_GET(pRecordData, &pRecordData->BlockLength);
				GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSyncPLL() Invalid BlockLength=%u ErrOffset=%u",
						BlockLen, ErrOffset);
			}
			else if (pRecordData->VersionHigh != GSY_SYNC_PLL_VERSIONHIGH)
			{
				ErrOffset = GSY_OFFSET_GET(pRecordData, &pRecordData->VersionHigh);
				GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSyncPLL() Invalid VersionHigh=%u ErrOffset=%u",
						pRecordData->VersionHigh, ErrOffset);
			}
			else if (pRecordData->VersionLow != GSY_SYNC_PLL_VERSIONLOW)
			{
				ErrOffset = GSY_OFFSET_GET(pRecordData, &pRecordData->VersionLow);
				GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSyncPLL() Invalid VersionLow=%u ErrOffset=%u",
						pRecordData->VersionLow, ErrOffset);
			}
			else
			{
				/* check record data
				*/
				ReductionRatio = GSY_SWAP16(pRecordData->ReductionRatio.Word);
				PLLHWDelay = GSY_SWAP16(pRecordData->PLLHWDelay.Word);
				ControlInterval = GSY_SWAP16(pRecordData->ControlInterval.Word);
				PLLWindow = GSY_SWAP32(pRecordData->PLLWindow.Dword);
				SetWaitCount = GSY_SWAP16(pRecordData->SetWaitCount.Word);
				Reserved = GSY_SWAP16(pRecordData->Reserved.Word);

				if ((ReductionRatio != 1) && (ReductionRatio != 2) && (ReductionRatio != 4) && (ReductionRatio != 8) && (ReductionRatio != 16))
				{
					ErrOffset = GSY_OFFSET_GET(pRecordData, &pRecordData->ReductionRatio);
					GSY_ERROR_TRACE_05(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSyncPLL() Invalid ReductionRatio=%u (Valid are %u, %u, %u, 8, 16) ErrOffset=%u",
							ReductionRatio, 1, 2, 4, ErrOffset);
				}
				else if ((PLLHWDelay < GSY_SYNC_PLL_DELAYMIN) || (PLLHWDelay > GSY_SYNC_PLL_DELAYMAX))
				{
					ErrOffset = GSY_OFFSET_GET(pRecordData, &pRecordData->PLLHWDelay);
					GSY_ERROR_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSyncPLL() Invalid PLLHWDelay=%u MIN=%u MAX=%u ErrOffset=%u",
							PLLHWDelay, GSY_SYNC_PLL_DELAYMIN, GSY_SYNC_PLL_DELAYMAX, ErrOffset);
				}
				else if (ControlInterval == 0)
				{
					ErrOffset = GSY_OFFSET_GET(pRecordData, &pRecordData->ControlInterval);
					GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSyncPLL() Invalid ControlInterval=%u ErrOffset=%u",
							ControlInterval, ErrOffset);
				}
				else if ((PLLWindow < 1) || (PLLWindow > GSY_SYNC_PLLWINDOW_MAX))
				{
					ErrOffset = GSY_OFFSET_GET(pRecordData, &pRecordData->PLLWindow);
					GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSyncPLL() Invalid PLLWindow=%u MIN=1 MAX=%u ErrOffset=%u",
							PLLWindow, GSY_SYNC_PLLWINDOW_MAX, ErrOffset);
				}
				else if (Reserved != 0)
				{
					ErrOffset = GSY_OFFSET_GET(pRecordData, &pRecordData->Reserved);
					GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSyncPLL() Invalid Reserved=%u ErrOffset=%u",
							Reserved, ErrOffset);
				}
				else
				{
					/* store record data and set PrmState
					*/
					pChUsr->PrmPLL = *pRecordData;
					pChUsr->PrmPLLWritten = LSA_TRUE;
					pChUsr->PrmState = GSY_PRM_STATE_WRITE;
					RespVal = GSY_RSP_OK;
					GSY_PRM_TRACE_05(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PrmWriteSyncPLL() ReductionRatio=%u PLLHWDelay=%u ControlInterval=%u PLLWindow=%u SetWaitCount=%u",
							ReductionRatio, PLLHWDelay, ControlInterval, PLLWindow, SetWaitCount);
				}
			}
		}
	}
	if (GSY_RSP_ERR_PRM_BLOCK == RespVal)
	{
		GSY_UPPER_RQB_PRMWRITE_ERROFFSET_SET(pRbu, ErrOffset);
	}

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_PrmWriteSyncPLL() pRbu=%X PrmState=%u RespVal=0x%x",
					pRbu, pChUsr->PrmState, RespVal);
	return(RespVal);
}

/*****************************************************************************/
/* Internal function: gsy_PrmValidName()                                     */
/* Check Subdomain name in PDSyncDAta Record like DCP station Name (CM)      */
/*****************************************************************************/
static LSA_BOOL  GSY_LOCAL_FCT_ATTR gsy_is_valid_port_name (
	LSA_UINT16 length,
	const GSY_UPPER_BYTE_PTR_TYPE name) 
{
	/*
	 * port-names have the following forms (see the definition of station-name below)
	 *
	 * "port-xyz" with x, y, z = 0...9
	 * "port-xyz-abcde" with x, y, z, a, b, c, d, e = 0...9
	 */

	if(
		length >= 8
		&& (name[0] == 'p' || name[0] == 'P') /* note: case insensitive, see caller */
		&& (name[1] == 'o' || name[1] == 'O')
		&& (name[2] == 'r' || name[2] == 'R')
		&& (name[3] == 't' || name[3] == 'T')
		&& name[4] == '-'
		&& (name[5] >= '0' && name[5] <= '9')
		&& (name[6] >= '0' && name[6] <= '9')
		&& (name[7] >= '0' && name[7] <= '9')
	) {

		if( length == 8 ) {
			return LSA_TRUE; /* matches with "port-xyz" with x, y, z = 0...9 */
		}

		if(
			length == 14
			&& name[8] == '-'
			&& (name[ 9] >= '0' && name[ 9] <= '9')
			&& (name[10] >= '0' && name[10] <= '9')
			&& (name[11] >= '0' && name[11] <= '9')
			&& (name[12] >= '0' && name[12] <= '9')
			&& (name[13] >= '0' && name[13] <= '9')
		) {
			return LSA_TRUE; /* matches with "port-xyz-abcde" with x, y, z, a, b, c, d, e = 0...9 */
		}
	}

	return LSA_FALSE; /* not a port-name */
}
/*****************************************************************************/
LSA_BOOL  GSY_LOCAL_FCT_ATTR gsy_PrmValidName (
GSY_CH_USR_PTR			pChUsr,
LSA_UINT16 				length,
GSY_UPPER_BYTE_PTR_TYPE name) 
{
	LSA_UINT16 idx;
	LSA_UINT16 lbl;
	LSA_UINT16 nr_of_labels;
	LSA_BOOL looks_like_ipaddr;

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_PrmValidName() pChUsr=%X length=%d name=0x%x", pChUsr, length, name);

	 /* Ascii check
	 */
	if (!('-' == 0x2d && '.' == 0x2e && '0' == 0x30 && 'a' == 0x61 && 'A' == 0x41 && '0' < '9' && 'a' < 'z' && 'A' < 'Z'))  //lint !e506 !e774 Constant Boolean within 'if' always evaluates to False, define-based behaviour, HM 20.05.2016
	{
		GSY_ERROR_TRACE_00(pChUsr->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** gsy_PrmValidName() not ASCII");
		gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
	}

	/*
	 * a station-name has the following syntax(see AP00216830):
	 * 1 or more labels, separated by [.]
	 * total length is 1-240
	 * label-length is 1-63
	 * labels consist of [a-z0-9-]
	 * labels do not start with [-]
	 * labels do not end with [-]
	 * the first (leftmost) label does not match with "port-xyz" with x, y, z = 0...9
	 * the first (leftmost) label does not match with "port-xyz-abcde" with x, y, z, a, b, c, d, e = 0...9
	 * station-names do not have the form n.n.n.n, n = 0...999
	 * labels do only start with 'xn-' if RFC 3490 is applied
	 */

	if(!(length >= 1 && length <= 240)) {
		return LSA_FALSE; /* station-name is too short or too long */
	}

	if(LSA_HOST_PTR_ARE_EQUAL(name, LSA_NULL)) {
		GSY_ERROR_TRACE_00(pChUsr->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** gsy_PrmValidName() name=LSA_NULL");
		gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
		return LSA_FALSE;       //lint !e527 Unreachable code, if gsy_ErrorInternal() is an exit (but this must not be), HM 10.06.2016
	}

	/***/

	nr_of_labels = 0;

	looks_like_ipaddr = LSA_TRUE;

	lbl = 0; /* start of label */

	for( idx = 0; idx <= length; ++idx ) {

		LSA_UINT8 ch;

		if( idx < length ) {
			ch = name[idx];
		}
		else {
			ch = '.'; /* fake a dot at the end */
		}

		/***/

		if( ch == '.' ) {

			LSA_UINT16 len = idx - lbl;

			if( len < 1 || len > 63 ) {
				return LSA_FALSE; /* the label is too short or too long */
			}

			if (idx == 0)
			{
				GSY_ERROR_TRACE_00(pChUsr->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** gsy_PrmValidName() idx=0");
				gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
			}

			if( name[idx - 1] == '-' ) {
				return LSA_FALSE; /* the label ends with '-' */
			}

			if( len > 3 ) {

				/* note: port-names are longer than 3 chars */

				looks_like_ipaddr = LSA_FALSE; /* the label has more than 3 characters */

				if(nr_of_labels == 0) {

					if( gsy_is_valid_port_name(len, &name[lbl]) ) {
						return LSA_FALSE; /* the first (leftmost) label matches a port-name */
					}
				}
			}

			nr_of_labels += 1;

			lbl = idx + 1; /* start of the next label */

			continue;
		}

		if( ch >= '0' && ch <= '9' ) {
			continue;
		}

		looks_like_ipaddr = LSA_FALSE; /* the label contains not only digits */

		if( ch >= 'a' && ch <= 'z' ) {
			continue;
		}

		if( ch >= 'A' && ch <= 'Z' ) {
			/* for backwards-compatibility: allow upper-case here, convert in cm_station_name_copy() */
			continue;
		}

		if( idx == lbl ) {
			return LSA_FALSE; /* the label does not start with a letter or digit */
		}

		if( ch == '-' ) {
			continue;
		}

		return LSA_FALSE; /* the label contains an illegal char */
	}

	if( looks_like_ipaddr && nr_of_labels == 4 ) {

		return LSA_FALSE; /* station-name looks like an ip-addr */
	}

	LSA_UNUSED_ARG(pChUsr);
	return LSA_TRUE; /* ok */
}

/*****************************************************************************/
/* Internal function: gsy_PrmWriteSyncData()                                 */
/* Write PDSyncData record to temporary Record (B)                           */
/*****************************************************************************/
static LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_PrmWriteSyncData(
GSY_CH_USR_PTR			pChUsr,
GSY_UPPER_RQB_PTR_TYPE  pRbu)
{
	LSA_UINT32	ErrOffset = GSY_PRM_ERR_OFFSET_DEFAULT;
	LSA_UINT32	HWType, HWMaster, SendInterval, PLLWindow, ResIntervalBegin, ResIntervalEnd, RecordLen, RecordIndex;
	LSA_UINT16	BlockType, Padding, SendClockFactor, SyncTimeout, SyncTakeover, PortId, SyncProperties, MasterStartupTime;
	LSA_UINT8	SyncRole, SyncId, MasterPrio, MasterLevel, MasterPrio1, MasterPrio2;
	LSA_UINT16	SyncDataLen = 0, CopyLen = 0, RespVal = GSY_RSP_ERR_SEQUENCE;
	GSY_UPPER_SYNC_DATA_PTR pSyncData = (GSY_UPPER_SYNC_DATA_PTR)(void*)GSY_UPPER_RQB_PRMRECORD_PTR_GET(pRbu);

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_PrmWriteSyncData() pChUsr=%X pRbu=%X SyncId=0x%02x",
					pChUsr, pRbu, pChUsr->SyncId);

	/* PortId check
	*/
	PortId = GSY_UPPER_RQB_PRMRECORD_PORT_GET(pRbu);
	RecordIndex = GSY_UPPER_RQB_PRMRECORD_IDX_GET(pRbu);
	if (0 != PortId)
	{
        RespVal = GSY_RSP_ERR_PRM_PORTID;
        GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSyncData() RecIdx=0x%x: Invalid port ID=%u",
				RecordIndex, PortId);
	}
	else
	{
		/* RecordLen check
		*/
		RespVal = GSY_RSP_ERR_PRM_DATA;
		RecordLen = GSY_UPPER_RQB_PRMRECORD_LEN_GET(pRbu);
		if ((RecordLen < GSY_BLOCK_TYPE_LEN_SIZE)
		||	(RecordLen != (LSA_UINT32)(GSY_BLOCK_TYPE_LEN_SIZE + GSY_SWAP16(pSyncData->BlockLength.Word))))
		{
			GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSyncData() RecIdx=0x%x: Invalid record length=%u",
					RecordIndex, RecordLen);
		}
		else
		{
			/* SyncData check part 1
			*/
			RespVal = GSY_RSP_ERR_PRM_BLOCK;
			BlockType = GSY_SWAP16(pSyncData->BlockType.Word);
			SyncDataLen = GSY_SWAP16(pSyncData->BlockLength.Word) + GSY_BLOCK_TYPE_LEN_SIZE;
			Padding = GSY_SWAP16(pSyncData->Padding1.Word);
			if (((GSY_SYNC_DATA_IDX == RecordIndex) && (GSY_SYNC_DATA_BLOCKTYPE != BlockType))
			||  ((GSY_SYNC_TIME_IDX == RecordIndex) && (GSY_SYNC_TIME_BLOCKTYPE != BlockType)))
			{
				ErrOffset = GSY_OFFSET_GET(pSyncData, &pSyncData->BlockType);
				GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSyncData() RecIdx=0x%x: Invalid BlockType=0x%04x ErrOffset=%u",
						RecordIndex, BlockType, ErrOffset);
			}
			else if ((SyncDataLen < (GSY_SYNC_DATA_LEN + GSY_SYNC_DATA_NAMELEN_MIN))
				  ||  (SyncDataLen > sizeof(GSY_SYNC_DATA_RECORD))
				  ||  (SyncDataLen > RecordLen))
			{
				ErrOffset = GSY_OFFSET_GET(pSyncData, &pSyncData->BlockLength);
				GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSyncData() RecIdx=0x%x: Invalid BlockLength=%u ErrOffset=%u",
						RecordIndex, SyncDataLen, ErrOffset);
			}
			else if (1 != pSyncData->VersionHigh)
			{
				ErrOffset = GSY_OFFSET_GET(pSyncData, &pSyncData->VersionHigh);
				GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSyncData() RecIdx=0x%x: Invalid VersionHigh=%u ErrOffset=%u",
						RecordIndex, pSyncData->VersionHigh, ErrOffset);
			}
			else if (2 != pSyncData->VersionLow)
			{
				ErrOffset = GSY_OFFSET_GET(pSyncData, &pSyncData->VersionLow);
				GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSyncData() RecIdx=0x%x: Invalid VersionLow=%u ErrOffset=%u",
						RecordIndex, pSyncData->VersionLow, ErrOffset);
			}
			else if (0 != Padding)
			{
				ErrOffset = GSY_OFFSET_GET(pSyncData, &pSyncData->Padding1);
				GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSyncData() RecIdx=0x%x: Invalid Padding1=%u ErrOffset=%u",
						RecordIndex, Padding, ErrOffset);
			}
			/* 290107lrg001: PDSyncData with SubdomainName since V1.1
			 * 170707lrg002: only V1.2 supported for RTSync
			*/
			else if ((GSY_SYNC_DATA_NAMELEN_MIN > pSyncData->LengthSubdomainName)
				  ||  (GSY_SYNC_DATA_NAMELEN_MAX < pSyncData->LengthSubdomainName)
				  ||  (SyncDataLen < (GSY_SYNC_DATA_LEN + pSyncData->LengthSubdomainName)))
			{
				ErrOffset = GSY_OFFSET_GET(pSyncData, &pSyncData->LengthSubdomainName);
				GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSyncData() RecIdx=0x%x: Invalid LengthSubdomainName=%u ErrOffset=%u",
						RecordIndex, pSyncData->LengthSubdomainName, ErrOffset);
			}
			else if (!gsy_PrmValidName(pChUsr, pSyncData->LengthSubdomainName, pSyncData->SubdomainName))
			{
				ErrOffset = GSY_OFFSET_GET(pSyncData, pSyncData->SubdomainName);
				GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSyncData() RecIdx=0x%x: Invalid SubdomainName ErrOffset=%u",
						RecordIndex, ErrOffset);
			}
			else
			{
				CopyLen = SyncDataLen;
			}
		}
	}
	if (CopyLen != 0)
	{
		/* SyncData check part 2
		*/
		ResIntervalBegin = GSY_SWAP32(pSyncData->ResIntervalBegin.Dword);
		ResIntervalEnd = GSY_SWAP32(pSyncData->ResIntervalEnd.Dword);
		PLLWindow = GSY_SWAP32(pSyncData->PLLWindow.Dword);
		SendInterval = GSY_SWAP32(pSyncData->SyncSendFactor.Dword);
		SendClockFactor = GSY_SWAP16(pSyncData->SendClockFactor.Word);
		SyncTimeout = GSY_SWAP16(pSyncData->TimeoutFactor.Word);
		SyncTakeover = GSY_SWAP16(pSyncData->TakeoverFactor.Word);
		MasterStartupTime = GSY_SWAP16(pSyncData->MasterStartupTime.Word);	// 290807lrg001
		SyncProperties = GSY_SWAP16(pSyncData->SyncProperties.Word);
		SyncRole = (LSA_UINT8)(SyncProperties & 0x03);
		SyncId = ((LSA_UINT8)(SyncProperties >> 8)) & GSY_SYNCID_MASK;
		HWMaster = pChUsr->pChSys->HardwareType & GSY_HW_TYPE_MASTER_MSK;	// 240409lrg002: HW master or SyncSend supportet?
		HWType = pChUsr->pChSys->HardwareType & GSY_HW_TYPE_MSK;			// TSyncOverPTCP
		MasterPrio1 = pSyncData->MasterPriority1;
		MasterLevel = GSY_SYNC_PRIO1_LEVEL(MasterPrio1);					// 170408lrg001
		MasterPrio = GSY_SYNC_PRIO1_PRIO(MasterPrio1);						// 170408lrg001
		MasterPrio2 = pSyncData->MasterPriority2;

		if (0 != ResIntervalBegin)
		{
			ErrOffset = GSY_OFFSET_GET(pSyncData, &pSyncData->ResIntervalBegin);
			GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSyncData() RecIdx=0x%x: Invalid ResIntervalBegin=%u ErrOffset=%u",
					RecordIndex, ResIntervalBegin, ErrOffset);
		}
		else if ((GSY_SYNC_TIME_IDX == RecordIndex) && (0 != ResIntervalEnd))
		{
			ErrOffset = GSY_OFFSET_GET(pSyncData, &pSyncData->ResIntervalEnd);
			GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSyncData() RecIdx=0x%x: Invalid ResIntervalEnd=%u ErrOffset=%u",
					RecordIndex, ResIntervalEnd, ErrOffset);
		}
		else if (GSY_SYNC_PLLWINDOW_MAX < PLLWindow)
		{
			ErrOffset = GSY_OFFSET_GET(pSyncData, &pSyncData->PLLWindow);
			GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSyncData() RecIdx=0x%x: Invalid PLLWindow=%u ErrOffset=%u",
					RecordIndex, PLLWindow, ErrOffset);
		}
		else if ((GSY_SYNC_SENDFACTOR_MIN > SendInterval) || (GSY_SYNC_SENDFACTOR_MAX < SendInterval))
		{
			ErrOffset = GSY_OFFSET_GET(pSyncData, &pSyncData->SyncSendFactor);
			GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSyncData() RecIdx=0x%x: Invalid SyncSendFactor=%u ErrOffset=%u",
					RecordIndex, SendInterval, ErrOffset);
		}
		else if ((GSY_SYNC_TIME_IDX == RecordIndex) && (0 != SendClockFactor))
		{
			ErrOffset = GSY_OFFSET_GET(pSyncData, &pSyncData->SendClockFactor);
			GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSyncData() RecIdx=0x%x: Invalid SendClockFactor=%u ErrOffset=%u",
					RecordIndex, SendClockFactor, ErrOffset);
		}
#if GSY_SYNC_TIMEOUT_MIN != 0
		else if ((GSY_SYNC_TIMEOUT_MAX < SyncTimeout) && (GSY_SYNC_TIMEOUT_MIN > SyncTimeout))
#else
		else if (GSY_SYNC_TIMEOUT_MAX < SyncTimeout)
#endif
		{
			ErrOffset = GSY_OFFSET_GET(pSyncData, &pSyncData->TimeoutFactor);
			GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSyncData() RecIdx=0x%x: Invalid TimeoutFactor=%u ErrOffset=%u",
					RecordIndex, SyncTimeout, ErrOffset);
		}
#if GSY_SYNC_TAKEOVER_MIN != 0
		else if (((GSY_SYNC_TAKEOVER_MAX < SyncTakeover) && (GSY_SYNC_TAKEOVER_MIN > SyncTakeover))
#else
		else if ((GSY_SYNC_TAKEOVER_MAX < SyncTakeover)
#endif
			 || ((GSY_SYNC_TIME_IDX == RecordIndex) && (0 != SyncTakeover)))
		{
			ErrOffset = GSY_OFFSET_GET(pSyncData, &pSyncData->TakeoverFactor);
			GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSyncData() RecIdx=0x%x: Invalid TakeoverFactor=%u ErrOffset=%u",
					RecordIndex, SyncTakeover, ErrOffset);
		}
#if GSY_SYNC_STARTUP_MIN != 0
		else if (((GSY_SYNC_STARTUP_MAX < MasterStartupTime) && (GSY_SYNC_STARTUP_MIN > MasterStartupTime))
#else
		else if ((GSY_SYNC_STARTUP_MAX < MasterStartupTime)
#endif
			 || ((GSY_SYNC_TIME_IDX == RecordIndex) && (0 != MasterStartupTime)))
		{
			ErrOffset = GSY_OFFSET_GET(pSyncData, &pSyncData->MasterStartupTime);
			GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSyncData() RecIdx=0x%x: Invalid MasterStartupTime=%u ErrOffset=%u",
					RecordIndex, MasterStartupTime, ErrOffset);
		}
		else if (((GSY_SYNC_TIME_IDX == RecordIndex) && (GSY_SYNCID_TIME != SyncId))
			 ||  ((GSY_SYNC_DATA_IDX == RecordIndex) && (GSY_SYNCID_CLOCK != SyncId))
			 ||  ((GSY_SYNC_ROLE_MASTER != SyncRole) && (GSY_SYNC_ROLE_SLAVE != SyncRole))
			 ||  ((GSY_SYNC_ROLE_MASTER == SyncRole) && (GSY_HW_TYPE_MASTER_NO == HWMaster))
			 ||  ((GSY_SYNC_TIME_IDX == RecordIndex) && (GSY_SYNC_ROLE_MASTER == SyncRole) && (GSY_HW_TYPE_SOC != HWType))
			 ||  ((GSY_SYNC_TIME_IDX == RecordIndex) && (GSY_SYNC_ROLE_SLAVE == SyncRole) && (GSY_HW_TYPE_ERTEC_200P != HWType) && (GSY_HW_TYPE_HERA != HWType)))
		{
			ErrOffset = GSY_OFFSET_GET(pSyncData, &pSyncData->SyncProperties);
			GSY_ERROR_TRACE_07(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSyncData() RecIdx=0x%x: Invalid SyncProperties=0x%04x (Id=%u Role=%u HWType=0x%08x HWMaster=0x%08x) ErrOffset=%u",
					RecordIndex, SyncProperties, SyncId, SyncRole, HWType, HWMaster, ErrOffset);
		}
		else if (((GSY_SYNC_ROLE_SLAVE == SyncRole) && (GSY_SYNC_PRIO1_SLAVE != MasterPrio1))	/* 170408lrg001: check */
			  ||  ((GSY_SYNC_ROLE_MASTER == SyncRole)
				&& ((GSY_SYNC_PRIO1_LEVEL_MAX < MasterLevel)			/* 170408lrg001 */
				  || ((GSY_SYNC_TIME_IDX == RecordIndex) && (GSY_SYNC_PRIO1_PRIMARY != MasterPrio))
				  || ((GSY_SYNC_PRIO1_PRIMARY != MasterPrio) && (GSY_SYNC_PRIO1_SECONDARY != MasterPrio)))))
		{
			ErrOffset = GSY_OFFSET_GET(pSyncData, &pSyncData->MasterPriority1);
			GSY_ERROR_TRACE_06(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSyncData() RecIdx=0x%x: Invalid MasterPriority1=0x%02x (Level=%u Prio=%u) Role=%u ErrOffset=%u",
					RecordIndex, MasterPrio1, MasterLevel, MasterPrio, SyncRole, ErrOffset);
		}
		else if ((GSY_SYNC_ROLE_MASTER == SyncRole) && (GSY_SYNC_PRIO2_DEFAULT != MasterPrio2))		/* 260706lrg001 */
		{
			ErrOffset = GSY_OFFSET_GET(pSyncData, &pSyncData->MasterPriority2);
			GSY_ERROR_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWriteSyncData() RecIdx=0x%x: Invalid MasterPriority2=0x%02x Role=%u ErrOffset=%u",
					RecordIndex, MasterPrio2, SyncRole, ErrOffset);
		}
		else
		{
			/* Copy SyncData Record into channel data and update PrmState
			*/
			GSY_COPY_UPPER_TO_LOCAL_MEM(pSyncData, &pChUsr->PrmData, CopyLen, pChUsr->pSys);
			pChUsr->PrmState = GSY_PRM_STATE_WRITE;
			if ((0 != pChUsr->PrmRecordIndexWritten) && (RecordIndex != pChUsr->PrmRecordIndexWritten))
			{
				pChUsr->PrmClockAndTimeWritten = LSA_TRUE;
			}
            pChUsr->PrmRecordIndexWritten = RecordIndex;
			RespVal = GSY_RSP_OK;
			GSY_PRM_TRACE_06(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PrmWriteSyncData(%02x) Len=%u: Role=%d Interval=%u Timeout=%u Window=%u",
							SyncId, CopyLen, SyncRole, SendInterval, SyncTimeout, PLLWindow);
			GSY_PRM_TRACE_06(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PrmWriteSyncData(%02x) Prio1.Level=%u Prio1.Prio=%u Prio2=%d Takeover=%u Startup=%u",
							SyncId, GSY_SYNC_PRIO1_LEVEL(MasterPrio1), GSY_SYNC_PRIO1_PRIO(MasterPrio1), MasterPrio2, SyncTakeover, MasterStartupTime);
		}
	}
	if (GSY_RSP_ERR_PRM_BLOCK == RespVal)
	{
		GSY_UPPER_RQB_PRMWRITE_ERROFFSET_SET(pRbu, ErrOffset);
	}

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_PrmWriteSyncData() pChUsr=%X RecordIndex=0x%x RespVal=0x%x",
					pChUsr, RecordIndex, RespVal);
	return(RespVal);
}

/*****************************************************************************/
/* Internal function: gsy_PrmWrite()                                         */
/* Write user record to temporary Record (B)                                 */
/*****************************************************************************/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_PrmWrite(
GSY_CH_USR_PTR			pChUsr,
GSY_UPPER_RQB_PTR_TYPE  pRbu)
{
	LSA_UINT32	RecordIndex = 0;
	LSA_UINT16	RespVal = GSY_RSP_ERR_SEQUENCE;
	GSY_UPPER_SYNC_DATA_PTR pRecordData = (GSY_UPPER_SYNC_DATA_PTR)(void*)GSY_UPPER_RQB_PRMRECORD_PTR_GET(pRbu);

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_PrmWrite() pChUsr=%X pRbu=%X SyncId=0x%02x",
					pChUsr, pRbu, pChUsr->SyncId);

	if (LSA_HOST_PTR_ARE_EQUAL(pRecordData, LSA_NULL))
	{
		RespVal = GSY_RSP_ERR_PARAM;
		GSY_ERROR_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWrite() Invalid record data pointer=%X",
				pRecordData);
	}
	else if ((GSY_PRM_STATE_PREPARE == pChUsr->PrmState)
	||  (GSY_PRM_STATE_WRITE == pChUsr->PrmState)
	||  (GSY_PRM_STATE_ERROR == pChUsr->PrmState))
	{
		/* If there already was a PrmPrepare or a PrmWrite...
		*/
		pChUsr->PrmState = GSY_PRM_STATE_ERROR;
		RespVal = GSY_RSP_ERR_PRM_INDEX;
		RecordIndex = GSY_UPPER_RQB_PRMRECORD_IDX_GET(pRbu);
		GSY_PRM_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PrmWrite(%02x) RecordIndex=0x%08x",
				pChUsr->SyncId, RecordIndex);

		if (GSY_SEND_CLOCK_IDX == RecordIndex)
		{
			/* 051206lrg001: load SendClockFactor record
			 * 131206lrg001: also on EDDS
			*/
			RespVal = gsy_PrmWriteSendClock(pChUsr, pRbu);
		}
		else
		{
			if (GSY_PORT_DATA_IDX == RecordIndex)
			{
				/* load PDPortDataAdjust record
				 * 170107lrg001: also on EDDS
				*/
				RespVal = gsy_PrmWritePortData(pChUsr, pRbu);
			}
			else if (pChUsr->pChSys->RxTxNanos != 0)
			{
				/* NOT on EDDS
				*/
				if (GSY_SYNC_PLL_IDX == RecordIndex)
				{
					/* 230910lrg001: load PDSyncPLL record
					*/
					RespVal = gsy_PrmWriteSyncPLL(pChUsr, pRbu);
				}
				else if (((GSY_SYNC_DATA_IDX == RecordIndex) && (1 == (GSY_SYNC_ID_SUPPORTED & 1)))    //lint !e506 Constant Boolean, define-based behaviour, HM 20.05.2016
					 ||  ((GSY_SYNC_TIME_IDX == RecordIndex) && (2 == (GSY_SYNC_ID_SUPPORTED & 2))))   //lint !e506 !e774 Constant Boolean within 'if' always evaluates to True, define-based behaviour, HM 20.05.2016
				{
					/* Check and write sync data/time record
					*/
					RespVal = gsy_PrmWriteSyncData(pChUsr, pRbu);
				}
				else
				{
					GSY_ERROR_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWrite() Invalid record index=0x%x",
								RecordIndex);
				}
			}
			else
			{
				GSY_ERROR_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWrite() No HW support for record index=0x%x",
							RecordIndex);
			}
		}
	}
	else
	{
		GSY_ERROR_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmWrite() Invalid PRM state=%u",
				pChUsr->PrmState);
	}

	if ((GSY_RSP_OK != RespVal) && (GSY_RSP_ERR_PRM_BLOCK != RespVal))
	{
		/* On GSY_RSP_ERR_PRM_BLOCK the error offset is already set
		*/
		GSY_UPPER_RQB_PRMWRITE_ERROFFSET_SET(pRbu, GSY_PRM_ERR_OFFSET_DEFAULT);
	}

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_PrmWrite() pChUsr=%X RecordIndex=0x%x RespVal=0x%x",
					pChUsr, RecordIndex, RespVal);
	return(RespVal);
}

/*****************************************************************************/
/* Internal function: gsy_PrmChangePort()                                    */
/*****************************************************************************/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_PrmChangePort(
GSY_CH_USR_PTR			pChUsr,
GSY_UPPER_RQB_PTR_TYPE  pRbu)
{
    #if (GSY_CFG_TRACE_MODE != 0)
    LSA_UINT16  PortID      = GSY_UPPER_RQB_PRMCHANGEPORT_PORTID_GET(pRbu);
    #endif
    LSA_UINT8   ModuleState = GSY_UPPER_RQB_PRMCHANGEPORT_MODULESTATE_GET(pRbu);
	LSA_UINT16  RespVal     = GSY_RSP_OK;

    #if (GSY_CFG_TRACE_MODE == 0)
        LSA_UNUSED_ARG(pChUsr);
    #endif

    GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_PrmChangePort() pChUsr=%X PortID=%d ModuleState=%d",
        pChUsr, PortID, ModuleState);

    switch (ModuleState)
    {
        case GSY_PRM_PORTMODULE_PLUG_PREPARE:
            GSY_FUNCTION_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"    gsy_PrmChangePort() PortID=%d ModuleState=GSY_PRM_PORTMODULE_PLUG_PREPARE", PortID);
            GSY_UPPER_RQB_PRMCHANGEPORT_PORTPARAMSNOTAPPLICABLE_SET(pRbu, GSY_PRM_PORT_PARAMS_APPLICABLE);
            break;
        case GSY_PRM_PORTMODULE_PLUG_COMMIT:
            GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"    gsy_PrmChangePort() PortID=%d ModuleState=GSY_PRM_PORTMODULE_PLUG_COMMIT ApplyDefaultPortparams=%d", 
                PortID, GSY_UPPER_RQB_PRMCHANGEPORT_APPLYDEFAULTPARAMS_GET(pRbu));
            break;
        case GSY_PRM_PORTMODULE_PULLED:
            GSY_FUNCTION_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"    gsy_PrmChangePort() PortID=%d ModuleState=GSY_PRM_PORTMODULE_PULLED", PortID);
            break;
        default:
            GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"    gsy_PrmChangePort() PortID=%d ModuleState=%d UNKNOWN", PortID, ModuleState);
            break;
    }
    
    GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_PrmChangePort() pChUsr=%X RespVal=0x%x",
                          pChUsr, RespVal);
    return(RespVal);
}

/*****************************************************************************/
/* Internal function: gsy_PrmEnd()                                           */
/* Check consistency of values in temporary records ueberpruefen             */
/*****************************************************************************/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_PrmEnd(
GSY_CH_USR_PTR			pChUsr,
GSY_UPPER_RQB_PTR_TYPE  pRbu)
{
	LSA_UINT16	SyncProperties;
	LSA_UINT8	SyncRole, SyncId, MasterPrio1;
	LSA_UINT16	RespVal = GSY_RSP_ERR_SEQUENCE;
	LSA_UINT32	ResIntervalEnd = GSY_SWAP32(pChUsr->PrmData.ResIntervalEnd.Dword);
	LSA_UINT32	SendClockNanos  = GSY_SWAP16(pChUsr->PrmSendClock.Word) * GSY_SYNC_CLOCK_BASE;	// 220107lrg002: was SWAP32
	LSA_UINT32	ErrOffset = GSY_PRM_ERR_OFFSET_DEFAULT;
	LSA_UINT32	ErrIndex = GSY_PRM_ERR_INDEX_DEFAULT;
    LSA_UINT8	PortparamsNotApplicable[EDD_CFG_MAX_PORT_CNT];

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_PrmEnd() pChUsr=%X pRbu=%X SyncId=0x%02x",
					pChUsr, pRbu, pChUsr->SyncId);

	/* If there already was a PrmPrepare or a PrmWrite...
	*/
	if ((GSY_PRM_STATE_PREPARE == pChUsr->PrmState)
	||  (GSY_PRM_STATE_WRITE == pChUsr->PrmState))
	{
		/* 051206lrg001: check SendClockFactor if SyncRecord has been written
		*/
		pChUsr->PrmState = GSY_PRM_STATE_ERROR;
		RespVal = GSY_RSP_ERR_PRM_CONSISTENCY;

		if ((pChUsr->PrmData.SendClockFactor.Word != 0)
		&&  (pChUsr->PrmData.SendClockFactor.Word != pChUsr->PrmSendClock.Word))
		{
			ErrOffset = GSY_OFFSET_GET(&pChUsr->PrmData, &pChUsr->PrmData.SendClockFactor);
			GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmEnd() SendClockFactor=%u does not match PDSyncData:%u ErrOffset=%u",
					GSY_SWAP16(pChUsr->PrmSendClock.Word), GSY_SWAP16(pChUsr->PrmData.SendClockFactor.Word), ErrOffset);
		}
		else if (ResIntervalEnd > SendClockNanos)
		{
			/* 131206lrg002: check ReservedIntervalEnd with SendClockFactor
			*/
			ErrOffset = GSY_OFFSET_GET(&pChUsr->PrmData, &pChUsr->PrmData.ResIntervalEnd);
			GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmEnd() Invalid ResIntervalEnd=%u SendClockNanos=%u ErrOffset=%u",
						ResIntervalEnd, SendClockNanos, ErrOffset);
		}
		else if (pChUsr->PrmClockAndTimeWritten)
		{
			GSY_ERROR_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmEnd() RecIdx=0x%x: Forbidden to overwrite the previous RecordIndex ",
					pChUsr->PrmRecordIndexWritten);
		}
		else
		{
			pChUsr->PrmRTC2 = LSA_FALSE;
			SyncProperties = GSY_SWAP16(pChUsr->PrmData.SyncProperties.Word);
			SyncRole = (LSA_UINT8)(SyncProperties & 0x03);
			SyncId = ((LSA_UINT8)(SyncProperties >> 8)) & GSY_SYNCID_MASK;
			MasterPrio1 = GSY_SYNC_PRIO1_PRIO(pChUsr->PrmData.MasterPriority1); 

			/* Check PDSyncPLL record
			*/
			if (pChUsr->PrmPLLWritten
			&&  (SyncRole != GSY_SYNC_ROLE_LOCAL)										/* PDSync record written */
			&&  ((SyncId != GSY_SYNCID_CLOCK) || (SyncRole != GSY_SYNC_ROLE_MASTER)))	//RQ 1533977 //131210lrg001
			{
				pChUsr->PrmPLLWritten = LSA_FALSE;
				ErrOffset = GSY_OFFSET_GET(&pChUsr->PrmData, &pChUsr->PrmData.SyncProperties);
				GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmEnd() Invalid SyncRole=%u or SyncId=%u for PDSyncPLL record ErrOffset=%u",
							SyncRole, SyncId, ErrOffset);
			}
			else if (pChUsr->PrmPLLWritten && (SyncRole == GSY_SYNC_ROLE_MASTER)
				  && (MasterPrio1 != GSY_SYNC_PRIO1_PRIMARY))
			{
				pChUsr->PrmPLLWritten = LSA_FALSE;
				ErrOffset = GSY_OFFSET_GET(&pChUsr->PrmData, &pChUsr->PrmData.MasterPriority1);
				GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmEnd() Invalid MasterPrio1=%u for PDSyncPLL record ErrOffset=%u",
							MasterPrio1, ErrOffset);
			}
			else 
			{
				//if the SyncId was changed
				pChUsr->PrmSyncIdChanged = ((SyncId != pChUsr->SyncId) ? LSA_TRUE : LSA_FALSE);

				RespVal = GSY_RSP_OK;
			}
		}
	}
	else
	{
		GSY_ERROR_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmEnd() Invalid PRM state=0x%x",
						pChUsr->PrmState);
	}

	if (GSY_RSP_OK != RespVal)
	{
		if (GSY_RSP_ERR_PRM_CONSISTENCY == RespVal)
		{
            if (0 != pChUsr->PrmRecordIndexWritten)
            {
                ErrIndex = pChUsr->PrmRecordIndexWritten;
            }
			GSY_UPPER_RQB_PRMEND_ERROFFSET_SET(pRbu, ErrOffset);
			GSY_UPPER_RQB_PRMEND_ERRINDEX_SET(pRbu, ErrIndex);
		}
		else
		{
			GSY_UPPER_RQB_PRMEND_ERROFFSET_SET(pRbu, GSY_PRM_ERR_OFFSET_DEFAULT);
			GSY_UPPER_RQB_PRMEND_ERRINDEX_SET(pRbu, GSY_PRM_ERR_INDEX_DEFAULT);
		}
		GSY_UPPER_RQB_PRMEND_ERRPORTID_SET(pRbu, GSY_PRM_ERR_PORT_ID_DEFAULT);
		GSY_UPPER_RQB_PRMEND_ERRFAULT_SET(pRbu, GSY_PRM_ERR_FAULT_DEFAULT);
	}
	else
	{
		GSY_MEMSET_LOCAL(PortparamsNotApplicable, GSY_PRM_PORT_PARAMS_APPLICABLE, sizeof(PortparamsNotApplicable));
		GSY_UPPER_RQB_PRMEND_PORTPARAMSNOTAPPLICABLE_SET(pRbu, PortparamsNotApplicable, pChUsr->pChSys->PortCount);

		pChUsr->PrmState = GSY_PRM_STATE_END;
		GSY_PRM_TRACE_06(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PrmEnd(%02x) PrmRTC2=%u ResEnd=%u SendClockNanos=%u PrmPLLWritten=%u PrmRecordIndexWritten=0x%x",
				pChUsr->SyncId, pChUsr->PrmRTC2, ResIntervalEnd, SendClockNanos, pChUsr->PrmPLLWritten, pChUsr->PrmRecordIndexWritten);
	}

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_PrmEnd() pChUsr=%X PrmRTC2=%d RespVal=0x%x",
					pChUsr, pChUsr->PrmRTC2, RespVal);

	LSA_UNUSED_ARG(pRbu);

	return(RespVal);
}

/*****************************************************************************/
/* Internal function: gsy_PrmCommit()                                        */
/* Put values of the temporary records (B) into active data (A)              */
/*****************************************************************************/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_PrmCommit(
GSY_CH_USR_PTR			pChUsr,
GSY_UPPER_RQB_PTR_TYPE  pRbu)
{
	LSA_UINT16	RespVal = GSY_RSP_ERR_SEQUENCE;
    LSA_UINT8	ApplyDefaultPortparams[EDD_CFG_MAX_PORT_CNT];

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_PrmCommit() pChUsr=%X pRbu=%X SyncId=0x%02x",
					pChUsr, pRbu, pChUsr->SyncId);

	/* If prior was a PrmEnd...
	*/
	if (GSY_PRM_STATE_END == pChUsr->PrmState)
	{
		RespVal = GSY_RSP_OK;

		/* 170707lrg002: not longer setting of bandwidth and -(de)activation
		*/

		/* 110806js001: restart synchronisation on subdomain UUID change
		*/

        GSY_UPPER_RQB_PRMCOMMIT_APPLYDEFAULTPORTPARAMS_GET(pRbu, ApplyDefaultPortparams, EDD_CFG_MAX_PORT_CNT);
#if (EDD_CFG_MAX_PORT_CNT >= 4)
        GSY_FUNCTION_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_PrmCommit() ApplyDefaultPortparams Port1=%d Port2=%d Port3=%d Port4=%d",
					ApplyDefaultPortparams[0], ApplyDefaultPortparams[1],ApplyDefaultPortparams[2],ApplyDefaultPortparams[3]);
#elif (EDD_CFG_MAX_PORT_CNT == 3)
        GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_PrmCommit() ApplyDefaultPortparams Port1=%d Port2=%d Port3=%d",
					ApplyDefaultPortparams[0], ApplyDefaultPortparams[1],ApplyDefaultPortparams[2]);
#elif (EDD_CFG_MAX_PORT_CNT == 2)
        GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_PrmCommit() ApplyDefaultPortparams Port1=%d Port2=%d",ApplyDefaultPortparams[0], ApplyDefaultPortparams[1]);
#elif (EDD_CFG_MAX_PORT_CNT == 1)
        GSY_FUNCTION_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_PrmCommit() ApplyDefaultPortparams Port1=%d", ApplyDefaultPortparams[0]);
#endif

		if (GSY_SUBDOMAIN_CMP(pChUsr->PrmData.SubdomainID, pChUsr->SyncData.SubdomainID))
		{
			pChUsr->PrmSubdomainIDChanged = LSA_FALSE;
			pChUsr->PrmSubdomainInd = LSA_FALSE;
		}
		else
		{
			pChUsr->PrmSubdomainIDChanged = LSA_TRUE;
			pChUsr->PrmSubdomainInd = LSA_TRUE;
		}

		/* Check PDSyncPLL and Data record for relevant changes
		*/
		if ((pChUsr->SyncPLLActive || pChUsr->PrmPLLWritten)
		&&  ((pChUsr->SyncPLL.ReductionRatio.Word != pChUsr->PrmPLL.ReductionRatio.Word)
		 ||  (pChUsr->SyncPLL.PLLHWDelay.Word != pChUsr->PrmPLL.PLLHWDelay.Word)
		 ||  (pChUsr->SyncPLL.ControlInterval.Word != pChUsr->PrmPLL.ControlInterval.Word)
//		 ||  (pChUsr->SyncData.PLLWindow.Dword != pChUsr->PrmData.PLLWindow.Dword)
//		 ||  (pChUsr->SyncData.SyncSendFactor.Dword != pChUsr->PrmData.SyncSendFactor.Dword)
//		 ||  (pChUsr->SyncData.SendClockFactor.Word != pChUsr->PrmData.SendClockFactor.Word)))
		 ||  (pChUsr->SendClockFactor.Word != pChUsr->PrmSendClock.Word)))
		{
			pChUsr->SyncPLLChanged = LSA_TRUE;
		}
		else
		{
			pChUsr->SyncPLLChanged = LSA_FALSE;
		}
		/* Copy SyncData/PLL Record and update PrmState
		*/
		pChUsr->SendClockFactor = pChUsr->PrmSendClock;
		pChUsr->SyncPLL = pChUsr->PrmPLL;
		pChUsr->SyncData = pChUsr->PrmData;
		pChUsr->PrmState = GSY_PRM_STATE_COMMIT;

		GSY_PRM_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PrmCommit(%02x) SubdomainIDChanged=%u SyncPLLChanged=%u RespVal=0x%x",
			pChUsr->SyncId, pChUsr->PrmSubdomainIDChanged, pChUsr->SyncPLLChanged, RespVal);

		gsy_PrmCommitNext(pChUsr, pRbu);
		RespVal = GSY_RSP_OK_ACTIVE;
	}
	else
	{
		GSY_ERROR_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmCommit() Invalid state=%u",
			pChUsr->PrmState);
	}

	GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_PrmCommit() pChUsr=%X RespVal=0x%x",
					pChUsr, RespVal);
	return(RespVal);
}

/*****************************************************************************/
/* Internal function: gsy_PrmCommitNext()                                    */
/* COMMIT state machine                                                      */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_PrmCommitNext(
GSY_CH_USR_PTR			pChUsr,
GSY_UPPER_RQB_PTR_TYPE  pRbu)
{
	GSY_SYNC_DATA_RECORD 	*pSyncData;
	LSA_UINT32	PLLWindow, SlaveWindow, SyncTimeout, SyncTakeover, TimeStamp = 0, SendInterval;
	LSA_UINT16	Timeout, Takeover, SyncInTime, SyncInterval, PLLInterval, Epoch = 0; 
	LSA_UINT16	MasterStartupTime, SyncProperties;
	LSA_UINT16	SetWaitCount, ReductionRatio, PLLHWDelay, SendClockFactor;		//230910lrg001
	LSA_UINT8	SyncRole, SyncId, MasterPrio1, MasterPrio2;
	LSA_UINT16	RetVal = LSA_RET_OK;
	LSA_UINT16	RespVal = GSY_RSP_ERR_SEQUENCE;
	LSA_BOOL	SendIndication = LSA_FALSE;

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_PrmCommitNext() pChUsr=%X pRbu=%X SyncId=0x%02x",
					pChUsr, pRbu, pChUsr->SyncId);

	GSY_PRM_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PrmCommitNext(%02x) PRM state=%u",
				pChUsr->SyncId, pChUsr->PrmState);

	/* If prior was a PrmCommit...
	*/
	if ((GSY_PRM_STATE_COMMIT == pChUsr->PrmState)
	||  (GSY_PRM_STATE_STOP == pChUsr->PrmState))
	{
		/* Get Sync parameters from active SyncDataRecord
		*/
		RespVal = GSY_RSP_OK;
		pSyncData = &pChUsr->SyncData;
		SlaveWindow = GSY_SWAP32(pSyncData->PLLWindow.Dword);
		SendInterval = GSY_SWAP32(pSyncData->SyncSendFactor.Dword);
		Timeout = GSY_SWAP16(pSyncData->TimeoutFactor.Word);
		Takeover = GSY_SWAP16(pSyncData->TakeoverFactor.Word);
		MasterStartupTime = GSY_SWAP16(pSyncData->MasterStartupTime.Word);	//290807lrg001
		SyncProperties = GSY_SWAP16(pSyncData->SyncProperties.Word);
		SyncRole = (LSA_UINT8)(SyncProperties & 0x03);
		SyncId = ((LSA_UINT8)(SyncProperties >> 8)) & GSY_SYNCID_MASK;
		MasterPrio1 = pSyncData->MasterPriority1;
		MasterPrio2 = pSyncData->MasterPriority2;

		/* Get PLL parameters from active SyncPLL record
		*/
		SendClockFactor = GSY_SWAP16(pChUsr->SendClockFactor.Word);
		ReductionRatio = GSY_SWAP16(pChUsr->SyncPLL.ReductionRatio.Word);
		PLLHWDelay = GSY_SWAP16(pChUsr->SyncPLL.PLLHWDelay.Word);
		PLLInterval = GSY_SWAP16(pChUsr->SyncPLL.ControlInterval.Word);
		PLLWindow = GSY_SWAP32(pChUsr->SyncPLL.PLLWindow.Dword);
		SetWaitCount = GSY_SWAP16(pChUsr->SyncPLL.SetWaitCount.Word);

		/* 210807lrg001: Timeout is counter instead muliple of 3125ns
		 *               Sync/SendInterval are muliple of 10ms (30...39 => 30)
		*/
		SyncTimeout = Timeout;
		SyncTakeover = Takeover;
		SendInterval = (SendInterval / GSY_CLOCK_BASE_PER_MILLISECOND);

		if (GSY_SYNC_ROLE_LOCAL != SyncRole)
		{
			if (SendInterval < 10)
			{
				SendInterval = 10;
				GSY_ERROR_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"*** gsy_PrmCommitNext() Increasing sync interval to %d milliseconds",
								SendInterval);
			}
			else if ((SendInterval % 10) != 0)
			{
				SendInterval = (SendInterval / 10) * 10;
				GSY_ERROR_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"*** gsy_PrmCommitNext() Decreasing sync interval to %d milliseconds",
								SendInterval);
			}
		}

		SyncInterval = (LSA_UINT16)SendInterval;
		SyncInTime = (LSA_UINT16)(SyncInterval * Timeout);

		if  ((pChUsr->SyncRole != SyncRole)
		||   (pChUsr->SyncId != SyncId)
		||   (pChUsr->PrmSubdomainIDChanged)				// 110806js001
		||   (pChUsr->SyncPLLChanged)						// 230910lrg001
		||   (pChUsr->Slave.SyncInterval != SyncInterval)
		||   (pChUsr->Master.SendInterval != (LSA_UINT16)SendInterval)
		||	 ((pChUsr->Master.Priority1 != MasterPrio1) && (GSY_SYNC_ROLE_MASTER == SyncRole)))	// 200308lrg001: Stop Master for Update Prio1
		{
			/* Stop/start Master, PLLsync or Slave depending of new SyncRole
			*/
			if ((GSY_PRM_STATE_STOP != pChUsr->PrmState)
			&&  ((GSY_MASTER_STATE_OFF != pChUsr->Master.State)
			 ||  (GSY_SLAVE_STATE_OFF != pChUsr->Slave.State)))
			{
				/* Stop Slave or Master
				*/
				pChUsr->PrmState = GSY_PRM_STATE_STOP;
				pChUsr->MasterControlState = GSY_MASTER_CONTROL_STATE_NONE;
				RespVal = gsy_SyncStop(pChUsr, pRbu);
				if ((GSY_RSP_OK != RespVal) && (GSY_RSP_OK_ACTIVE != RespVal))
				{
					GSY_ERROR_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmCommitNext() Error=%x stopping master/slave/PLL",
									RespVal);
					gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_CONFIG, pRbu, sizeof(pRbu));
				}
				GSY_PRM_TRACE_05(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PrmCommitNext(%02x) Sync stop RespVal=0x%x SlaveStatate=%u MasterState=%u PLLState=%u", 
							pChUsr->SyncId, RespVal, pChUsr->Slave.State, pChUsr->Master.State, pChUsr->PLLData.State);
			}
			else if (pChUsr->SyncPLLChanged
			&&  (GSY_PLL_STATE_SYNC_OFF != pChUsr->PLLData.State))
			{
				/* Stop PLL synchronisation
				*/
				GSY_PRM_TRACE_05(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PrmCommitNext(%02x) PLL stop PrmState=0x%x SlaveStatate=%u MasterState=%u PLLState=%u", 
							pChUsr->SyncId, pChUsr->PrmState, pChUsr->Slave.State, pChUsr->Master.State, pChUsr->PLLData.State);

				if (!LSA_HOST_PTR_ARE_EQUAL(pRbu, LSA_NULL))
				{
			        gsy_PutUpperQ(&pChUsr->QueueUpper, pRbu);
				}
				RespVal = GSY_RSP_OK_ACTIVE;
				pChUsr->PLLData.State = GSY_PLL_STATE_SYNC_STOP;
				pChUsr->PrmState = GSY_PRM_STATE_STOP;

				if (GSY_HW_TYPE_SLAVE_HW == (pChUsr->pChSys->HardwareType & GSY_HW_TYPE_SLAVE_MSK))
				{
					/* Stop PLL synchronisation in KRISC32
					*/
				}
				/* else all will be done by gsy_MasterPLLTimer()
				*/
			}

			if (GSY_RSP_OK == RespVal) 
			{
			 	if (pChUsr->SyncPLLChanged)
				{
					GSY_PRM_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PrmCommitNext(%02x) SyncPLLChanged: PrmPLLWritten=%u", 
								pChUsr->SyncId, pChUsr->PrmPLLWritten);

					pChUsr->PLLData.ReductionRatio = ReductionRatio;
					pChUsr->PLLData.PLLHWDelay = PLLHWDelay;
					pChUsr->PLLData.PLLInterval = PLLInterval;
					pChUsr->PLLData.PLLWindow = PLLWindow;
					pChUsr->PLLData.SetWaitCount = SetWaitCount;
					if (pChUsr->PrmPLLWritten)
					{
						/* Start PLL synchronisation
						*/
						pChUsr->PLLData.CycleLength = (LSA_UINT32)SendClockFactor * GSY_SYNC_CLOCK_BASE;
						pChUsr->SyncPLLActive = LSA_TRUE;
						if (GSY_HW_TYPE_SLAVE_HW == (pChUsr->pChSys->HardwareType & GSY_HW_TYPE_SLAVE_MSK))
						{
							/* Start PLL synchronisation in KRISC32
							*/
						}
						else
						{
							/* Start PLL synchronisation in GSY
							*/
							gsy_MasterPLLStart(pChUsr);
						}
					}
					else
					{
						/* Not longer PLL synchronisation
						*/
						pChUsr->PLLData.CycleLength = 0;
						pChUsr->SyncPLLActive = LSA_FALSE;
					}
					pChUsr->SyncPLLChanged = LSA_FALSE;
				}

				if (GSY_SYNC_ROLE_LOCAL != SyncRole)
				{
					/* Now there is no running service and a new master or slave should be started: 
					 * Reset statistik data on (re)start of master/slave
					 * 100209lrg001: do not get time on EDDP-SyncChannel
					*/
					GSY_MEMSET_LOCAL(&pChUsr->Statistics, 0, sizeof(GSY_SYNC_STATISTIC_TYPE));
					if (GSY_HW_TYPE_SLAVE_SW == (pChUsr->pChSys->HardwareType & GSY_HW_TYPE_SLAVE_MSK))
					{
						RetVal = GSY_SYNC_GET(&Epoch, &pChUsr->Statistics.StatisticAge.Seconds, 
											  &pChUsr->Statistics.StatisticAge.Nanoseconds, 
											  &TimeStamp, SyncId, pChUsr->pSys);
					}
					if (LSA_RET_OK != RetVal)
					{
						pChUsr->PrmState = GSY_PRM_STATE_ERROR;
						RespVal = GSY_RSP_ERR_PARAM;
						GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmCommitNext() SyncId=%u: Error=0x%x from GSY_SYNC_GET()",
										SyncId, RetVal);
						gsy_ErrorUser(GSY_MODULE_ID, __LINE__, GSY_ERR_CONFIG, pRbu, sizeof(pRbu));
					}
					else
					{
						/* Reset Indication flags/event and set Sync parameters
						*/
						pChUsr->Event.OldEvent = GSY_SYNC_EVENT_NONE;
						pChUsr->Event.IndFlags = GSY_PRM_IND_FLAGS_NONE;
						if (GSY_SYNCID_CLOCK == SyncId)
						{
							pChUsr->Event.Prm.ChannelErrorType = GSY_PRM_CHANNEL_ERROR_TYPE_SYNC;
						}
						else
						{
							pChUsr->Event.Prm.ChannelErrorType = GSY_PRM_CHANNEL_ERROR_TYPE_TIME;
						}
						pChUsr->SyncId = SyncId;
						pChUsr->Slave.WindowSize = SlaveWindow;
						pChUsr->Slave.SyncInTime = SyncInTime;
						pChUsr->Slave.SyncTimeout = SyncTimeout;
						pChUsr->Slave.SyncTakeover = SyncTakeover;
						pChUsr->Master.Priority1 = MasterPrio1;
						pChUsr->Master.Priority2 = MasterPrio2;
						pChUsr->Master.StartupTime = MasterStartupTime;

						/* Start Master or Slave
						*/
						pChUsr->SyncRole = SyncRole;
						pChUsr->Master.SendInterval = (LSA_UINT16)SendInterval;
						pChUsr->Slave.SyncInterval = SyncInterval;
						pChUsr->PrmSubdomainIDChanged = LSA_FALSE;  /* 110806js001 */

						pChUsr->PrmState = GSY_PRM_STATE_START;
						if (GSY_SYNC_ROLE_MASTER == SyncRole)
						{
							if  ( ((pChUsr->pChSys->HardwareType & GSY_HW_TYPE_MSK) == GSY_HW_TYPE_SOC)
								&& (pChUsr->SyncId == GSY_SYNCID_TIME) )
							{
								pChUsr->MasterControlState = GSY_MASTER_CONTROL_STATE_STARTABLE;
								SendIndication = LSA_TRUE;
							}
							else
							{
								RespVal = gsy_MasterStart(pChUsr);
							}
						}
						else if (GSY_SYNC_ROLE_SLAVE == SyncRole)
							RespVal = gsy_SlaveStart(pChUsr);
						GSY_PRM_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PrmCommitNext(%02x) SyncRole=%u SyncPLLActive=%u: start RespVal=0x%x", 
									pChUsr->SyncId, SyncRole, pChUsr->SyncPLLActive, RespVal);
						if (GSY_RSP_OK != RespVal)
						{
							GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmCommitNext() SyncId=%u: Error=0x%x starting master/slave",
											pChUsr->SyncId, RespVal);
							gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_CONFIG, pRbu, sizeof(pRbu));
						}
					}
				}
				else
				{
					/* No new service after stop: 
					 * Reset Indication flags/event and set Sync parameters
					*/
					SendIndication = LSA_TRUE;
					pChUsr->PrmState = GSY_PRM_STATE_STOP;
					pChUsr->Event.SyncEvent = GSY_SYNC_EVENT_NONE;		// 291106lrg001
					pChUsr->Event.OldEvent = GSY_SYNC_EVENT_NONE;
					pChUsr->Event.IndFlags = GSY_PRM_IND_FLAGS_NONE;
					pChUsr->Event.PrmFlags = GSY_PRM_IND_FLAGS_NONE;
					pChUsr->Slave.WindowSize = SlaveWindow;
					pChUsr->Slave.SyncInterval = SyncInterval;
					pChUsr->Slave.SyncInTime = SyncInTime;
					pChUsr->Slave.SyncTimeout = SyncTimeout;
					pChUsr->Slave.SyncTakeover = SyncTakeover;
					pChUsr->Master.Priority1 = MasterPrio1;
					pChUsr->Master.Priority2 = MasterPrio2;
					pChUsr->Master.SendInterval = (LSA_UINT16)SendInterval;
					pChUsr->Master.StartupTime = MasterStartupTime;
					pChUsr->SyncId = GSY_SYNCID_NONE;
					pChUsr->SyncRole = SyncRole;
					GSY_PRM_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PrmCommitNext(%02x) SyncRole local after stop", 
								pChUsr->SyncId);
				}
			}
		}
		else
		{
			if (pChUsr->PrmPLLWritten)
			{
				/* Update PLL synchronisation parameters
				*/
				pChUsr->PLLData.ReductionRatio = ReductionRatio;
				pChUsr->PLLData.PLLHWDelay = PLLHWDelay;
                pChUsr->PLLData.PLLInterval = PLLInterval;
				pChUsr->PLLData.PLLWindow = PLLWindow;
				pChUsr->PLLData.SetWaitCount = SetWaitCount;
				pChUsr->PLLData.CycleLength = (LSA_UINT32)SendClockFactor * GSY_SYNC_CLOCK_BASE;

				GSY_PRM_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PrmCommitNext(%02x) PrmPLLWritten: update PLLData", 
							pChUsr->SyncId);

			}
			if (GSY_SYNC_ROLE_LOCAL != SyncRole)
			{
				/* Old master/slave service remains running:
				 * Reset Indication flags/event and set Sync parameters
				*/
				LSA_UINT8 MasterMode = GSY_LOWER_MASTER_OFF;
				LSA_BOOL HwUpdate = LSA_FALSE;

				if ((GSY_HW_TYPE_SLAVE_HW == (pChUsr->pChSys->HardwareType & GSY_HW_TYPE_SLAVE_MSK))
				&&  ((GSY_SYNCID_CLOCK == SyncId) || (GSY_SYNCID_TIME == SyncId))
				&&  ((pChUsr->Slave.WindowSize != SlaveWindow)
				 ||  (pChUsr->Slave.SyncInTime != SyncInTime)
				 ||  (pChUsr->Slave.SyncTimeout != SyncTimeout)
				 ||  (pChUsr->Slave.SyncTakeover != SyncTakeover)
				 ||  (pChUsr->Master.Priority2 != MasterPrio2)))
				{
					if (GSY_SYNC_ROLE_MASTER == SyncRole)
					{
						if (pChUsr->Master.Priority2 != MasterPrio2)
							MasterMode = GSY_LOWER_MASTER_UPDATE;
						else
							MasterMode = GSY_LOWER_MASTER_ACTIVE;
					}
					HwUpdate = LSA_TRUE;
				}

				pChUsr->PrmState = GSY_PRM_STATE_NONE;
				pChUsr->Event.OldEvent = GSY_SYNC_EVENT_NONE;
				pChUsr->Event.IndFlags = GSY_PRM_IND_FLAGS_NONE;
				pChUsr->Event.PrmFlags = GSY_PRM_IND_FLAGS_NONE;
				pChUsr->SyncId = SyncId;
				pChUsr->Slave.WindowSize = SlaveWindow;
				pChUsr->Slave.SyncInTime = SyncInTime;
				pChUsr->Slave.SyncTimeout = SyncTimeout;
				pChUsr->Slave.SyncTakeover = SyncTakeover;
				pChUsr->Master.Priority2 = MasterPrio2;
				pChUsr->Master.StartupTime = MasterStartupTime;
				if (GSY_SYNC_ROLE_MASTER == SyncRole)
				{
					/* 200308lrg001: Update	running Master
					*/
					if (pChUsr->Master.Priority1 != MasterPrio1)
					{
						GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmCommitNext() Master for SyncId=%u not stopped changing Prio1 from 0x%x to 0x%x", 
								pChUsr->SyncId, pChUsr->Master.Priority1, MasterPrio1);
						pChUsr->Master.Priority1 = MasterPrio1;
						MasterPrio1 = pChUsr->Master.LocalMaster.Priority1 & GSY_SYNC_PRIO1_ACTIVE;
						MasterPrio1 |= pChUsr->Master.Priority1;
						pChUsr->Master.LocalMaster.Priority1 = MasterPrio1;
					}
					pChUsr->Master.LocalMaster.Priority2 = pChUsr->Master.Priority2;
					if (GSY_SYNC_TAKEOVER_DISABLED != pChUsr->Slave.SyncTakeover)
						pChUsr->Master.TimeoutCount = pChUsr->Slave.SyncTakeover;
					else
						pChUsr->Master.TimeoutCount = pChUsr->Slave.SyncTimeout;
				}
				/* If sync is running in hardware...
				*/
				if (HwUpdate)
				{
					/* Update KRISC32 slave and master by sending a RQB to EDDP
					*/
					gsy_MasterSlaveControl(pChUsr, GSY_LOWER_SLAVE_UPDATE, MasterMode);
				}
			}
			else
			{
				/* No old and no new service: 
				 * 070706lrg001: initiate empty PRM Indication
				*/
				pChUsr->Event.SyncEvent = GSY_SYNC_EVENT_NONE;
			}
			SendIndication = LSA_TRUE;
			GSY_PRM_TRACE_07(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PrmCommitNext(%02x) SyncRole=%u not changed: Prio1.Level=%u Prio1.Prio=%u Window=%u SyncInTime=%u Timeout=%u", 
						pChUsr->SyncId, SyncRole, GSY_SYNC_PRIO1_LEVEL(MasterPrio1), GSY_SYNC_PRIO1_PRIO(MasterPrio1), SlaveWindow, SyncInTime, SyncTimeout);
		}

		if (GSY_RSP_OK_ACTIVE != RespVal)
		{
			/*  Change Forwarding configuration from PDPortDataAdjust record
			*/
			pChUsr->pChSys->pCommitRbu = pRbu;
			pChUsr->pChSys->PrmFwdInd = SendIndication;
			gsy_PrmCommitEnd(pChUsr->pChSys, GSY_RSP_OK);
		}
	}

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_PrmCommitNext() pChUsr=%X PrmState=0x%x RespVal=0x%x",
					pChUsr, pChUsr->PrmState, RespVal);
}

/*****************************************************************************/
/* Internal access function: gsy_PrmCommitEnd()                              */
/* Finish COMMIT state machine                                               */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_PrmCommitEnd(
GSY_CH_SYS_PTR			pChSys,
LSA_UINT16				RespVal)
{
	LSA_UINT16 			PortIdx;
	LSA_UINT32 			BitBoundary, SyncIdMaskIn, SyncIdMaskOut;
	GSY_DRIFT_PTR		pDriftClock = LSA_NULL;
	GSY_DRIFT_PTR		pDriftTime  = LSA_NULL;
	GSY_MAC_ADR_TYPE	NoMacAddr = {GSY_NO_MASTER_ADDR};


	LSA_BOOL			PortDataAdjustWritten = LSA_TRUE;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_PrmCommitEnd() pChUsr=%X pRbu=%X RespVal=0x%x",
					pChSys->pChUsr, pChSys->pCommitRbu, RespVal);

	GSY_PRM_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PrmCommitEnd(%02x) RespVal=0x%x SendIndication=%u RxTxNanos=%u", 
				pChSys->pChUsr->SyncId, RespVal, pChSys->PrmFwdInd, pChSys->RxTxNanos);

	if (GSY_RSP_OK != RespVal)
	{
		GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmCommitEnd() SyncId=%u: Error=0x%x written FWD configuration",
			pChSys->pChUsr->SyncId, RespVal);
		gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_CONFIG, LSA_NULL, 0);
	}
	/* 301106lrg001: Do not configure Forwarding an EDD without timestamps
	*/
	else if (pChSys->RxTxNanos != 0)
	{
		for (PortIdx = 0; PortIdx < pChSys->PortCount; PortIdx++)
		{

			/* 161106js001: has port to be configured?
			*/
			if (pChSys->Port[PortIdx].PrmDomainCfg)
			{
				/* 161106js001: take values from record or default values
				*/
				if (pChSys->Port[PortIdx].PrmDomainWritten)
				{
					/* 161106js001: load PRM record as actual record and set state to CFG
					 *              DomainBoundary contains the PRM record on GSY_PRM_PORT_STATE_CFG
					*/

					pChSys->Port[PortIdx].PortDataAdjust = pChSys->Port[PortIdx].PrmPortDataAdjust; 	
					pChSys->Port[PortIdx].DomainBoundary = pChSys->Port[PortIdx].PrmDomainBoundary; 	
					pChSys->Port[PortIdx].PrmDomainState = GSY_PRM_PORT_STATE_CFG;
					pChSys->Port[PortIdx].PrmDomainWritten = LSA_FALSE;
					PortDataAdjustWritten = LSA_TRUE;

					/* Build Forwarding Mode by negation of PrmDomainBoundary
					*/

					if (pChSys->Port[PortIdx].DomainBoundary.VersionLow == 1)
					{
						SyncIdMaskIn = ~GSY_SWAP32(pChSys->Port[PortIdx].PrmDomainBoundary.Boundary.Domain1.DomainBoundaryIngress.Dword);
						SyncIdMaskOut = ~GSY_SWAP32(pChSys->Port[PortIdx].PrmDomainBoundary.Boundary.Domain1.DomainBoundaryEgress.Dword);
					}
					else
					{
						SyncIdMaskIn = ~GSY_SWAP32(pChSys->Port[PortIdx].PrmDomainBoundary.Boundary.Domain.DomainBoundary.Dword);
						SyncIdMaskOut = SyncIdMaskIn;
						GSY_PRM_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"*** gsy_PrmCommitEnd(%02x) Port=%u: using domain boundary 0x%08x for IN and OUT",
							pChSys->pChUsr->SyncId, PortIdx+1, ~SyncIdMaskOut);
					}
				}
				else
				{
					/* 2049154: No actual Record present
					 * Default: All is ON
					*/
					pChSys->Port[PortIdx].PrmDomainState = GSY_PRM_PORT_STATE_INI;
					SyncIdMaskIn = 0;	// AP01382171: gsy_usr.c(4426): warning C4245: '=' : conversion from 'int' to 'compiler_switch_type_uint32', signed/unsigned mismatch
					SyncIdMaskIn = ~SyncIdMaskIn;
					SyncIdMaskOut = SyncIdMaskIn;
				}
				 /*	161106js001: configure port forwarding now
				  * -> because of asynchronous return of gsy_FwdPortForwardCtrl()
				 */
				pChSys->Port[PortIdx].PrmDomainCfg = LSA_FALSE;

				pChSys->PortFwd[PortIdx].FwdMode = SyncIdMaskOut;
				pChSys->PortInp[PortIdx].FwdMode = SyncIdMaskIn;
				GSY_PRM_TRACE_05(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PrmCommitEnd(%02x) pChSys=%X Port=%u SyncIdMaskIn=0x%08x SyncIdMaskOut=0x%08x",
							pChSys->pChUsr->SyncId, pChSys, PortIdx+1, SyncIdMaskIn, SyncIdMaskOut);
				RespVal = gsy_FwdPortForwardCtrl(pChSys, (LSA_UINT16)(PortIdx+1), SyncIdMaskIn, SyncIdMaskOut, gsy_PrmCommitEnd);
				if (GSY_RSP_OK != RespVal)
					break;
			}
		}

		if ((GSY_RSP_OK != RespVal) && (GSY_RSP_OK_ACTIVE != RespVal))
		{
			GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_PrmCommitEnd() SyncId=%u: Error=0x%x writing FWD configuration",
							pChSys->pChUsr->SyncId, RespVal);
			gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_CONFIG, LSA_NULL, 0);
		}
	}

	if (GSY_RSP_OK == RespVal) 
	{
		/*  Callback Upper Commit-RQB and start delay measurement at all ports
		*/
		gsy_CallbackUsr(pChSys->pChUsr, pChSys->pCommitRbu, GSY_RSP_OK);

		/* 301106lrg001: Do not start delay measurement an EDD without timestamps
		 * 190608lrg001: but store record on EDDS
		if (pChSys->RxTxNanos != 0)
		*/
		{
			for (PortIdx = 0; PortIdx < pChSys->PortCount; PortIdx++)
			{
				/* 2049154: Default: no DelayBoundary is set
				*/
				pChSys->Port[PortIdx].DelayBoundary = LSA_FALSE;
				pChSys->Port[PortIdx].PrmPeerState = GSY_PRM_PORT_STATE_INI;

				if (pChSys->Port[PortIdx].PrmPeerWritten)
				{
					/* Copy Subblock into buffer (A) and update DelayBoundary
					*/
					pChSys->Port[PortIdx].PrmPeerState = GSY_PRM_PORT_STATE_CFG;
					pChSys->Port[PortIdx].PeerBoundary = pChSys->Port[PortIdx].PrmPeerBoundary;
					BitBoundary = GSY_SWAP32(pChSys->Port[PortIdx].PeerBoundary.Boundary.Peer.PeerBoundary.Dword);
					GSY_PRM_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PrmCommitEnd(%02x) pChSys=%X Port=%u PeerBoundary=0x%08x",
						pChSys->pChUsr->SyncId, pChSys, PortIdx+1, BitBoundary);
					if (BitBoundary & 2)
					{
						/* Bit1 = 1 stopps the delay measurement
						*/
						pChSys->Port[PortIdx].DelayBoundary = LSA_TRUE;
					}
					if (!PortDataAdjustWritten)
						pChSys->Port[PortIdx].PortDataAdjust = pChSys->Port[PortIdx].PrmPortDataAdjust; 	
				}
				if (pChSys->RxTxNanos != 0)
				{
					/* 190608lrg001: Do not start delay measurement an EDD without timestamps
					*/
					GSY_PRM_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PrmCommitEnd(%02x) pChSys=%X Port=%u DelayBoundary=%u",
						pChSys->pChUsr->SyncId, pChSys, PortIdx+1, pChSys->Port[PortIdx].DelayBoundary);
					if (pChSys->Port[PortIdx].DelayBoundary)
						gsy_DelayUserCtrl(pChSys, (LSA_UINT16)(PortIdx+1), GSY_DELAY_STOP);
					else
						gsy_DelayUserCtrl(pChSys, (LSA_UINT16)(PortIdx+1), GSY_DELAY_START);
				}
			}
		}

		if (pChSys->PrmFwdInd)
		{
			/* Current service remains running:
			 * repeat last Indication as  acknowledgment after Callback
			 * 110806js001: bandwidth possibly has to be activated -> LSA_TRUE
			 *
			*/
			gsy_SyncUserInd(pChSys->pChUsr, LSA_TRUE, LSA_TRUE);
		}

		if (  (pChSys->pChUsr->PrmSyncIdChanged) 
        ||  ( (pChSys->pChUsr->PrmSubdomainInd)
		 &&  ((GSY_HW_TYPE_SLAVE_HW != (pChSys->HardwareType & GSY_HW_TYPE_SLAVE_MSK))) )  )
		{
			if (1 == (GSY_SYNC_ID_SUPPORTED & 1))       //lint !e506 !e774 Constant Boolean within 'if' always evaluates to True, define-based behaviour, HM 20.05.2016
			{	// SyncId 0 is supported
				pDriftClock = pChSys->Drift[GSY_SYNCID_CLOCK];
				gsy_PrmCommitEndDiag(pChSys, pDriftClock, GSY_SYNCID_CLOCK);
			}
			if (2 == (GSY_SYNC_ID_SUPPORTED & 2))       //lint !e506 !e774 Constant Boolean within 'if' always evaluates to True, define-based behaviour, HM 20.05.2016
			{	// SyncId 1 is supported
				pDriftTime = pChSys->Drift[GSY_SYNCID_TIME];
				gsy_PrmCommitEndDiag(pChSys, pDriftTime, GSY_SYNCID_TIME);
			}

			GSY_PRM_TRACE_05(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PrmCommitEnd(%02x) pDriftClock=%X pDriftTime=%X PrmSyncIdChanged=%u PrmSubdomainInd=%u",
				pChSys->pChUsr->SyncId, pDriftClock, pDriftTime, pChSys->pChUsr->PrmSyncIdChanged, pChSys->pChUsr->PrmSubdomainInd);

            switch (pChSys->pChUsr->SyncId)
			{
				case GSY_SYNCID_CLOCK:
					if (GSY_SYNCID_CLOCK != pDriftClock->Diag.SyncId)   //SynId has changed
					{
						pDriftClock->Diag.SyncId = GSY_SYNCID_CLOCK;
						if (!LSA_HOST_PTR_ARE_EQUAL(pDriftTime, LSA_NULL))      //lint !e774 Boolean within 'if' always evaluates to True, it depends on the define GSY_SYNC_ID_SUPPORTED, HM 20.05.2016
						{
							pDriftTime->Diag.SyncId = GSY_SYNCID_NONE;
							pDriftTime->Diag.MasterMacAddr = NoMacAddr;
						}
					}
			        gsy_DiagUserInd(GSY_DIAG_SOURCE_SUBDOMAIN, pChSys, pDriftClock, 
			    			0      /* LSA_UINT32: MasterSeconds */, 
							0      /* LSA_UINT32: MasterNanoseconds */, 
							0      /* LSA_INT32: Offset */, 
							0      /* LSA_INT32: AdjustInterval */, 
							0      /* LSA_INT32: UserValue1 */, 
							0      /* LSA_INT32: UserValue2 */, 
							0      /* LSA_INT32: SetTimeHigh */, 
							0      /* LSA_INT32: SetTimeLow */, 
							0	   /* LSA_INT16: PortId */);
					break;

				case GSY_SYNCID_TIME:
					if (GSY_SYNCID_TIME != pDriftTime->Diag.SyncId)     //SynId has changed
					{
						pDriftTime->Diag.SyncId = GSY_SYNCID_TIME;
						if (!LSA_HOST_PTR_ARE_EQUAL(pDriftClock, LSA_NULL))      //lint !e774 Boolean within 'if' always evaluates to True, it depends on the define GSY_SYNC_ID_SUPPORTED, HM 20.05.2016
						{
							pDriftClock->Diag.SyncId = GSY_SYNCID_NONE;
							pDriftClock->Diag.MasterMacAddr = NoMacAddr;
						}
					}
			        gsy_DiagUserInd(GSY_DIAG_SOURCE_SUBDOMAIN, pChSys, pDriftTime, 
			    			0      /* LSA_UINT32: MasterSeconds */, 
							0      /* LSA_UINT32: MasterNanoseconds */, 
							0      /* LSA_INT32: Offset */, 
							0      /* LSA_INT32: AdjustInterval */, 
							0      /* LSA_INT32: UserValue1 */, 
							0      /* LSA_INT32: UserValue2 */, 
							0      /* LSA_INT32: SetTimeHigh */, 
							0      /* LSA_INT32: SetTimeLow */, 
							0	   /* LSA_INT16: PortId */);
					break;

				default:
					if (!LSA_HOST_PTR_ARE_EQUAL(pDriftClock, LSA_NULL))      //lint !e774 Boolean within 'if' always evaluates to True, it depends on the define GSY_SYNC_ID_SUPPORTED, HM 20.05.2016
					{
						pDriftClock->Diag.SyncId = GSY_SYNCID_NONE;
						pDriftClock->Diag.MasterMacAddr = NoMacAddr;
					}
					if (!LSA_HOST_PTR_ARE_EQUAL(pDriftTime, LSA_NULL))      //lint !e774 Boolean within 'if' always evaluates to True, it depends on the define GSY_SYNC_ID_SUPPORTED, HM 20.05.2016
					{
						pDriftTime->Diag.SyncId = GSY_SYNCID_NONE;
						pDriftTime->Diag.MasterMacAddr = NoMacAddr;
					}
					break;
			}

            pChSys->pChUsr->PrmSubdomainInd = LSA_FALSE;
			pChSys->pChUsr->PrmSyncIdChanged = LSA_FALSE;
		}
		RespVal = gsy_PrmPrepare(pChSys->pChUsr, LSA_NULL);
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_PrmCommitEnd() pChUsr=%X PrmState=0x%x RespVal=0x%x",
					pChSys->pChUsr, pChSys->pChUsr->PrmState, RespVal);
}

/*****************************************************************************/
/* Internal access function: gsy_PrmCommitEndDiag()                          */
/* Send subdomain diagnosis to user                                          */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_PrmCommitEndDiag(
GSY_CH_SYS_PTR			pChSys,
GSY_DRIFT_PTR			pDrift,
LSA_UINT8				DriftId)
{
	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_PrmCommitEndDiag() pChSys=%X pDrift=%X",
					pChSys, pDrift);

	pDrift->Diag.Subdomain = LSA_FALSE;
	/* 221106lrg001: update Diagnosis data and send Indication
	 * 061009lrg001: not on CLOCK or TIME sync if in harware
	*/
	if (!GSY_SUBDOMAIN_IS_NULL(pChSys->pChUsr->SyncData.SubdomainID))
	{
		if (((GSY_SYNCID_CLOCK == pChSys->pChUsr->SyncId) || (GSY_SYNCID_TIME == pChSys->pChUsr->SyncId))
		&&  (DriftId == pChSys->pChUsr->SyncId))
		{
			pDrift->Diag.Subdomain  = LSA_TRUE;
		}
	}
	/* 150107lrg001: enable drift measurement to new master on subdomain change
	 * 070108lrg003: first set Diag.Subdomain because of diagnosis in gsy_DriftSet() 
	 * 131210lrg001: no drift reset if PLL sync is running
	 * 051015lrg001: no drift reset if slave runs in hardware
	*/
	if (((GSY_SYNCID_CLOCK == pDrift->Diag.SyncId) || (GSY_SYNCID_TIME == pDrift->Diag.SyncId))
	&&  (GSY_HW_TYPE_SLAVE_HW != (pChSys->HardwareType & GSY_HW_TYPE_SLAVE_MSK))
	&& !pChSys->pChUsr->SyncPLLActive)
	{
		gsy_DriftReset(pChSys, pDrift, pChSys->pChUsr->SyncId);
	}

	pDrift->Diag.StateSync = LSA_FALSE;
	if ((GSY_SLAVE_STATE_SYNC == pChSys->pChUsr->Slave.State)
	||  (GSY_MASTER_STATE_PRIMARY == pChSys->pChUsr->Master.State))
	{
		if ((GSY_SYNCID_CLOCK == pChSys->pChUsr->SyncId) || (GSY_SYNCID_TIME == pChSys->pChUsr->SyncId))
		{
			pDrift->Diag.StateSync = LSA_TRUE;
		}
	}

    //In case of changing the SyncId, 
    //it is neccessary to clean out the previous diagnosis BEFORE Diag.SyncId is set to GSY_SYNCID_NONE
    gsy_DiagUserInd(GSY_DIAG_SOURCE_SUBDOMAIN, pChSys, pDrift, 
			    	0      /* LSA_UINT32: MasterSeconds */, 
					0      /* LSA_UINT32: MasterNanoseconds */, 
					0      /* LSA_INT32: Offset */, 
					0      /* LSA_INT32: AdjustInterval */, 
					0      /* LSA_INT32: UserValue1 */, 
					0      /* LSA_INT32: UserValue2 */, 
					0      /* LSA_INT32: SetTimeHigh */, 
					0      /* LSA_INT32: SetTimeLow */, 
					0	   /* LSA_INT16: PortId */);

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_PrmCommitEndDiag() SyncId=%u Diag.Subdomain=%u Diag.StateSync=%u",
					pChSys->pChUsr->SyncId, pDrift->Diag.Subdomain, pDrift->Diag.StateSync);
}

/*****************************************************************************/
/* Internal access function: gsy_Rtc2Cnf()                                   */
/* Confirmation of requests for RTClass2 control                             */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_Rtc2Cnf(
GSY_CH_SYS_PTR			pChSys,
GSY_LOWER_RQB_PTR_TYPE  pRbl)
{
	GSY_UPPER_RQB_PTR_TYPE  pRbs;
	GSY_UPPER_RQB_PTR_TYPE  pRbu = (GSY_UPPER_RQB_PTR_TYPE)(GSY_LOWER_RQB_USER_PTR_GET(pRbl));
	GSY_CH_USR_PTR			pChUsr = pChSys->pChUsr;
	GSY_LOWER_SERVICE_TYPE  Service = GSY_LOWER_RQB_SERVICE_GET(pRbl);
	LSA_UINT16				RespVal, Response = GSY_LOWER_RQB_RESPONSE_GET(pRbl);

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_Rtc2Cnf() pChSys=%X pRbl=%X pChUsr=%X",
					pChSys, pRbl, pChUsr);

	GSY_SYNC_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_Rtc2Cnf() Service=0x%x Response=0x%x", Service, Response);

	if (!LSA_HOST_PTR_ARE_EQUAL(pChUsr, LSA_NULL)
	&&  ((GSY_SYNCID_CLOCK == pChUsr->SyncId) || pChUsr->SyncPLLActive))
	{
		if (Response == GSY_LOWER_RSP_OK)
			Response = GSY_RSP_OK;
		else 
		{
			GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"*** gsy_Rtc2Cnf() Response=0x%x from lower service=0x%x",
					Response, Service);

            /* 030807JS: no error expected
            */
			gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_RTC2, pRbl, sizeof(pRbl));
		}

		if (GSY_LOWER_TIME_SET == Service)
		{
			/* The setting of time is done:
			 * store Lower SyncSet RQB in channel structure and update Sync state
			 * 121010lrg001: update PLL state
			*/
			pChUsr->pSyncSetRbl = pRbl;
			if (GSY_PLL_STATE_SYNC_WAIT_SET == pChUsr->PLLData.State)
			{
				if (0 != pChUsr->PLLData.SetWaitCount)
				{
					pChUsr->PLLData.ActWaitCount = 0;
					pChUsr->PLLData.State = GSY_PLL_STATE_SYNC_WAIT_OK;
					GSY_SYNC_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_Rtc2Cnf(%02x) SetWaitCount=%u: PLL state change ->WAIT_OK", 
									pChUsr->SyncId, pChUsr->PLLData.SetWaitCount);
				}
				else
				{
					pChUsr->PLLData.State = GSY_PLL_STATE_SYNC_SET;
					GSY_SYNC_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_Rtc2Cnf(%02x) PLL state change ->SET", pChUsr->SyncId);
				}
			}
			else if (GSY_SLAVE_STATE_WAIT_SET == pChUsr->Slave.State)		//230107lrg001: nicht nach STOP.
			{
				pChUsr->Slave.State = GSY_SLAVE_STATE_SET;
				GSY_SYNC_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_Rtc2Cnf(%02x) slave state change ->SET", pChUsr->SyncId);
			}

			/* Handle Upper SyncParams RQBs if queued because of GSY_SYNC_SET()
			*/
			while (pChUsr->QueueWait.Count != 0)
			{
				pRbs = pChUsr->QueueWait.pFirst;
				gsy_DequeUpper(&pChUsr->QueueWait, pRbs);
				RespVal = gsy_SyncParams(pChUsr, pRbs);
				gsy_CallbackUsr(pChUsr, pRbs, RespVal);
			}
		}
		else
		{
			/*  Store Lower RTC2 RQB in channel structure
			*/
			gsy_EnqueLower(&pChUsr->qRTC2Rbl, pRbl);
		}

		if ((GSY_MASTER_STATE_STOPPING == pChUsr->Master.State)
		||  (GSY_SLAVE_STATE_STOPPING == pChUsr->Slave.State))
		{
				/* 190907lrg001: finish Master and Slave
				*/

			if (GSY_SLAVE_STATE_STOPPING == pChUsr->Slave.State)
			{
				/* 240806lrg002: Slave stopped
				*/
				gsy_SlaveStop(pChUsr);
			}
			if (GSY_MASTER_STATE_STOPPING == pChUsr->Master.State)
			{
				/* 240806lrg002: Master stopped
				*/
				gsy_MasterStop(pChUsr);
			}
		}
		else 
		{
			if (!LSA_HOST_PTR_ARE_EQUAL(pRbu, LSA_NULL))
			{
				if (GSY_PRM_STATE_COMMIT == pChUsr->PrmState)
				{
					gsy_PrmCommitNext(pChUsr, pRbu);
				}
				else
				{
					/*  Callback Upper RQB
					*/
					gsy_CallbackUsr(pChUsr, pRbu, Response);
				}
			}

			if (GSY_LOWER_SYNC_SET == Service)
			{
				/* 120207lrg001: callback PRM Indication RQBs waiting for lower confirmation
				*/
				while (pChUsr->QueueEvent.Count != 0)
				{
					pRbu = pChUsr->QueueEvent.pFirst;
					gsy_DequeUpper(&pChUsr->QueueEvent, pRbu);
					gsy_CallbackUsr(pChUsr, pRbu, GSY_RSP_OK);
				}
			}
		}
	}
	else
	{
		GSY_ERROR_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** gsy_Rtc2Cnf() no user channel on pChSys=%X pChUsr=%X pRbl=%X Service=0x%x", pChSys, pChUsr, pRbl, Service);
		gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_UID, pRbl, (LSA_UINT32)pChSys);
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_Rtc2Cnf() pChSys=%X Service=0x%x Response=0x%x",
					pChSys, Service, Response);
}

/*****************************************************************************/
/* Internal function: gsy_UserDelayGet()                                     */
/* Write current Cable-Delay to upper RQB                                    */
/*****************************************************************************/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_UserDelayGet(
GSY_CH_SYS_PTR			pChSys,
GSY_UPPER_RQB_PTR_TYPE	pRbu)
{
	LSA_UINT16 RespVal = GSY_RSP_ERR_PARAM;
	LSA_UINT16 PortId = GSY_UPPER_RQB_DELAY_PORT_GET(pRbu);
	LSA_UINT16 Idx = 0;

	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_UserDelayGet() pChSys=%X Port=%u",
					pChSys, PortId);

	if ((PortId > 0)
	&&  (PortId <= pChSys->PortCount))
	{
		Idx = PortId - 1;
		if (pChSys->Port[Idx].DelayReqActive)
		{
			GSY_UPPER_RQB_DELAY_TIME_SET(pRbu, pChSys->Port[Idx].CableDelay);
			GSY_UPPER_RQB_DELAY_MIN_SET(pRbu, pChSys->Port[Idx].MinDelay);
			GSY_UPPER_RQB_DELAY_MAX_SET(pRbu, pChSys->Port[Idx].MaxDelay);
			GSY_UPPER_RQB_DELAY_COUNT_SET(pRbu, pChSys->Port[Idx].SumCount);
			RespVal = GSY_RSP_OK;
		}
		else
			RespVal = GSY_RSP_ERR_SEQUENCE;
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_UserDelayGet() pRbu=%X CableDelay=%d RespVal=0x%x",
						pRbu, pChSys->Port[Idx].CableDelay, RespVal);
	return(RespVal);
}

/*****************************************************************************/
/* Internal access function: gsy_UserDelayInd()                              */
/* Indicate Line-Delay on all user channels of the system channel            */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_UserDelayInd(
GSY_CH_SYS_PTR		pChSys,
LSA_UINT16			PortId,
LSA_UINT16			LowerRsp)
{
	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_UserDelayInd() pChSys=%X Port=%u LineDelay=%u",
						pChSys, PortId, pChSys->Port[PortId-1].SyncLineDelay);

#ifdef GSY_MESSAGE
	/*  **** TODO **** currently not used
	*/
#endif /* GSY_MESSAGE */

	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_UserDelayInd() pChSys=%X LowerRsp=0x%x",
					pChSys, LowerRsp);

	LSA_UNUSED_ARG(pChSys);
	LSA_UNUSED_ARG(PortId);
	LSA_UNUSED_ARG(LowerRsp);
}

/*****************************************************************************/
/* Internal access function: gsy_FwdFrameUserForward()                       */
/* Receive sync, FU or announce frame from forwarding                        */
/*****************************************************************************/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_FwdFrameUserForward(
GSY_CH_SYS_PTR          pChSys,
GSY_LOWER_RQB_PTR_TYPE  pRbl,
GSY_TLV_INFO_PTR_TYPE   pTLVInfo)
{
	GSY_UPPER_RQB_PTR_TYPE  pRbu;
	GSY_PTCP_DATA			PtcpData;
	GSY_PTCP_DATA			*pPtcpData = &PtcpData;
    LSA_UINT16				PtcpLen = 0;
	GSY_LOWER_RXBYTE_PTR	pMemRx = (GSY_LOWER_RXBYTE_PTR)GSY_LOWER_RQB_RECV_PTR_GET(pRbl);
    LSA_UINT16				RxLen = GSY_LOWER_RQB_RECV_DATA_LEN_GET(pRbl);
    LSA_UINT16				Offset = GSY_LOWER_RQB_RECV_DATA_OFFSET_GET(pRbl);
	LSA_UINT32				TimeStamp = GSY_LOWER_RQB_RECV_TIME_GET(pRbl);
	GSY_LOWER_RX_PDU_PTR	pPduRx = (GSY_LOWER_RX_PDU_PTR)(void*)(pMemRx + Offset + 2);
	GSY_LOWER_RXWORD_PTR	pFrameId = (GSY_LOWER_RXWORD_PTR)(void*)(pMemRx + Offset);
    LSA_UINT16				PduFrameId = *pFrameId;
	GSY_UPPER_WORD_PTR_TYPE pUserData;
    LSA_UINT16		Length;
	GSY_CH_USR_PTR  pChUsr = pChSys->pChUsr;
	LSA_UINT16		PortId = GSY_LOWER_RQB_RECV_PORT_GET(pRbl);
    LSA_UINT16		RetVal = GSY_RET_OK;
    LSA_UINT16		Response = GSY_RSP_OK;
    #if (GSY_CFG_TRACE_MODE != 0)
    LSA_UINT8		SyncId = GSY_SYNCID_GET_FROM_PDU(pFrameId);
    #endif

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_FwdFrameUserForward() pChSys=%X pRbl=%X pTLVInfo=%X",
					pChSys, pRbl, pTLVInfo);

	if (LSA_HOST_PTR_ARE_EQUAL(pChUsr, LSA_NULL))
	{
		/* No user channel for indication ?!?
		*/
		RetVal = GSY_RET_ERR_PARAM;
		GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** gsy_FwdFrameUserForward() pChSys=%X No user channel for SyncId=%u",
					pChSys, SyncId);
	}
	else
	{
		/* Copy PTCP Payload of frame to local memory
		*/
		switch (pTLVInfo->Type)
		{
		case GSY_TLV_TYPE_SYNC:
			PtcpData.Sync = pPduRx->Sync;
			PtcpLen = GSY_SYNC_RX_SIZE;
			break;

		case GSY_TLV_TYPE_FOLLOWUP:
			PtcpData.SyncFu = pPduRx->SyncFu;
			PtcpLen = GSY_SYNC_FU_RX_SIZE;
			break;

		case GSY_TLV_TYPE_ANNOUNCE:
			PtcpData.Announce = pPduRx->Announce;
			PtcpLen = GSY_ANNOUNCE_RX_SIZE;
			break;

		default:
			RetVal = GSY_RET_ERR_PARAM;
			GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** gsy_FwdFrameUserForward() pChSys=%X Unkwown Frame type received: 0x%x",
						pChSys, pTLVInfo->Type);
		}
	}

	/* Return lower RQB quickly to lower Layer
	*/
	if (GSY_TLV_TYPE_ANNOUNCE == pTLVInfo->Type)
		gsy_AnnoRecv(pChSys, pRbl, GSY_LOWER_RQB_USERID_UVAR32_GET(pRbl));
	else
		gsy_SyncRecv(pChSys, pRbl, GSY_LOWER_RQB_USERID_UVAR32_GET(pRbl));


	if (GSY_RET_OK == RetVal)
	{
        GSY_IS_VALID_PTR(pChUsr);

		/* Pass local PTCP Payload to Sync Slave or Master (BMA)
		*/
		switch (pTLVInfo->Type)
		{
		case GSY_TLV_TYPE_SYNC:
			if (GSY_SUBDOMAIN_CMP(pPtcpData->Sync.TLV.Subdomain.SubdomainUUID, pChUsr->SyncData.SubdomainID))
			{
				if ((GSY_PLL_STATE_SYNC_OFF == pChUsr->PLLData.State)
				||  (GSY_PLL_STATE_SYNC_SLEEP == pChUsr->PLLData.State))
				{
					if ((pChUsr->SyncId == GSY_SYNCID_CLOCK)
					&&  (GSY_HW_TYPE_I210 == (pChUsr->pChSys->HardwareType & GSY_HW_TYPE_MSK)))
					{
						/* lrgi210: PTCP time is inserted in front of frame by hardware
						*/
						pChUsr->Slave.Act.LocalNanos = *(LSA_UINT32*)(void*)(pMemRx - 8);
						pChUsr->Slave.Act.LocalSeconds = *(LSA_UINT32*)(void*)(pMemRx - 4);
					}
					gsy_SlaveIndSync(pChUsr, pPtcpData, pTLVInfo, PortId, TimeStamp);
				}
				if (GSY_MASTER_STATE_OFF != pChUsr->Master.State)
				{
					/* Trigger Master state machine
					 * 301110lrg001: indication from slave
					gsy_MasterIndSync(pChUsr, pTLVInfo, PortId);
					*/
				}
			}
			else
			{
				/* Invalid Subdomain
				*/
				pChUsr->Slave.SyncError = GSY_SYNC_EVENT_WRONG_PTCP_SUBDOMAIN_ID;
				pChUsr->Statistics.FrameWrongSubdomain++;
				GSY_SYNC_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"*** gsy_FwdFrameUserForward(%02x) Sync Subdomain mismatch: SeqId=%u",
								pChUsr->SyncId, pTLVInfo->Param.Sync.SequenceID);
			}
			break;

		case GSY_TLV_TYPE_FOLLOWUP:
			if ((GSY_PLL_STATE_SYNC_OFF == pChUsr->PLLData.State)
			||  (GSY_PLL_STATE_SYNC_SLEEP == pChUsr->PLLData.State))
			{
				gsy_SlaveIndFu(pChUsr, pPtcpData, pTLVInfo, PortId);
			}
			break;

		case GSY_TLV_TYPE_ANNOUNCE:
			if ((GSY_MASTER_STATE_OFF != pChUsr->Master.State)
			&&  GSY_SUBDOMAIN_CMP(pPtcpData->Announce.Subdomain.SubdomainUUID, pChUsr->SyncData.SubdomainID))
			{
				/* 041007lrg002: Announce only to Master if running
				 * 250108lrg001: check Subdomain of Announce
				 * 100308lrg001: check MasterMAC of Announce pruefen 171109lrg001: already checked in TLVGetInfo()
				*/
				gsy_MasterIndAnno(pChUsr, pTLVInfo, PortId);
			}
			else
			{
				/* Ignor other frames
				*/
				GSY_SYNC_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"*** gsy_FwdFrameUserForward(%02x) MasterState=%u(OFF) or Announce Subdomain mismatch: SeqId=%u",
								pChUsr->SyncId, pChUsr->Master.State, GSY_SWAP16(PtcpData.Announce.PtcpHdr.SequenceID.Word));
			}
			break;

		default:
			RetVal = GSY_RET_ERR_PARAM;
			GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** gsy_FwdFrameUserForward() pChSys=%X Unkwown Frame type received: 0x%x",
						pChSys, pTLVInfo->Type);
		}

		if (pChUsr->QueueRcv.Count)
		{
			/* Indicate local PTCP Payload to User
			*/
			pRbu = pChUsr->QueueRcv.pFirst;
	        gsy_DequeUpper(&pChUsr->QueueRcv, pRbu);

			/* Copy receive data and FrameId to user buffer and set length in RQB
			 * 131008lrg001: GSY_UPPER_WORD_PTR_TYPE
			*/
			pUserData = (GSY_UPPER_WORD_PTR_TYPE)GSY_UPPER_RQB_RECV_DATA_PTR_GET(pRbu);
			Length = GSY_UPPER_RQB_RECV_DATA_LEN_GET(pRbu);
			RxLen = PtcpLen + 2;
			if (RxLen > Length)
			{
				RxLen = Length;
				Response = GSY_RSP_OK_MORE;
			}
			if (RxLen >= 2)
			{
				*pUserData = PduFrameId;
				pUserData++;
				GSY_COPY_LOCAL_TO_UPPER_MEM(pPtcpData, pUserData, (LSA_UINT16)(RxLen-2), pChUsr->pSys);
			}
			else
				RxLen = 0;

			GSY_UPPER_RQB_RECV_DATA_LEN_SET(pRbu, RxLen);

			/* Write LineDelay and timestamp to RQB
			*/
			GSY_UPPER_RQB_RECV_DATA_LINEDELAY_SET(pRbu, 0);
			if (GSY_TLV_TYPE_SYNC == pTLVInfo->Type)
			{
				/* Sync frame
				*/
				switch (pChSys->HardwareType & GSY_HW_TYPE_MSK) 
				{
				case GSY_HW_TYPE_ERTEC_200:
				case GSY_HW_TYPE_SOC:
					/* LineDelay of ports has already been added
					*/
					break;

				case GSY_HW_TYPE_ERTEC_400:
				default:
					/* LineDelay of port has do be added
					*/
					GSY_UPPER_RQB_RECV_DATA_LINEDELAY_SET(pRbu, pChUsr->pChSys->Port[PortId-1].SyncLineDelay);
				}
				GSY_UPPER_RQB_RECV_DATA_TIMESTAMP_SET(pRbu, TimeStamp);
				GSY_UPPER_RQB_RECV_DATA_RESOLUTION_SET(pRbu, pChSys->RxTxNanos);
			}
			else
			{
				/* FollowUp frame or unknown
				*/
				GSY_UPPER_RQB_RECV_DATA_TIMESTAMP_SET(pRbu, 0);
				GSY_UPPER_RQB_RECV_DATA_RESOLUTION_SET(pRbu, 0);
			}
			/* Callback Upper receive indication RQB
			*/
			gsy_CallbackUsr(pChUsr, pRbu, Response);
		}
	}

	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_FwdFrameUserForward() pChSys=%X RetVal=0x%x",
					pChSys, RetVal);
    return(RetVal);
}

/*****************************************************************************/
/* Internal function: gsy_MasterControl()                                    */
/* Requests the start or stop of the PTCP time master                        */
/*****************************************************************************/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_MasterControl(
GSY_CH_USR_PTR			pChUsr,
GSY_UPPER_RQB_PTR_TYPE  pRbu)
{
    LSA_UINT32  Mode        = GSY_UPPER_RQB_MASTER_CONTROL_MODE_GET(pRbu);
    LSA_UINT16  UTCOffset   = GSY_UPPER_RQB_MASTER_CONTROL_UTCOFFSET_GET(pRbu);
    LSA_UINT16  RespVal     = GSY_RSP_OK;

    GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_MasterControl() pChUsr=%X Mode=%d UTCOffset=%d",
        pChUsr, Mode, UTCOffset);

    if  ( ((pChUsr->pChSys->HardwareType & GSY_HW_TYPE_MSK) != GSY_HW_TYPE_SOC)
        || (pChUsr->SyncId != GSY_SYNCID_TIME) )
    {
        RespVal = GSY_RSP_ERR_CONFIG;
        GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** gsy_MasterControl() not allowed HardwareType=0x%x SyncId=%d",
            pChUsr->pChSys->HardwareType, pChUsr->SyncId);
    }
    else if (pChUsr->SyncRole != GSY_SYNC_ROLE_MASTER)
    {
        RespVal = GSY_RSP_ERR_SEQUENCE;
        GSY_ERROR_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** gsy_MasterControl() not allowed SyncRole=%d",
            pChUsr->SyncRole);
    }
    else if (pChUsr->MasterControlState == GSY_MASTER_CONTROL_STATE_NONE)
    {
        RespVal = GSY_RSP_ERR_SEQUENCE;
        GSY_ERROR_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** gsy_MasterControl() not allowed MasterControlState=0x%x",
            pChUsr->MasterControlState);
    }
    else switch (Mode)
    {
        case GSY_MASTER_START:
            pChUsr->Master.CurrentUTCOffset = UTCOffset;
            if (pChUsr->MasterControlState != GSY_MASTER_CONTROL_STATE_STARTED)
            {
                pChUsr->MasterControlState = GSY_MASTER_CONTROL_STATE_STARTED;
                RespVal = gsy_MasterStart(pChUsr);
            }
            break;
        case GSY_MASTER_STOP:
            if (pChUsr->MasterControlState == GSY_MASTER_CONTROL_STATE_STARTED)
            {
                pChUsr->MasterControlState = GSY_MASTER_CONTROL_STATE_STARTABLE;
                RespVal = gsy_SyncStop(pChUsr, pRbu);
            }
            break;
        default:
            RespVal = GSY_RSP_ERR_PARAM;
            GSY_ERROR_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** gsy_MasterControl() invalid Mode=%d",
                Mode);
            break;
    }

    GSY_FUNCTION_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_MasterControl() pRbu=%X Mode=%d UTCOffset=%d RespVal=0x%x",
        pRbu, Mode, UTCOffset, RespVal);
    return(RespVal);
}

/*****************************************************************************/
/*  end of file GSY_USR.C                                                    */
/*****************************************************************************/

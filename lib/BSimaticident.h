#ifndef BSIMATICIDENT_H
#define BSIMATICIDENT_H

#include <QByteArray>
#include <QDebug>
#include <QList>
#include <QObject>
#include <QTimer>
#include <QSerialPort>


class BSimaticIdent : public QObject
{
    Q_OBJECT

public:
	enum LEDColor : char {
		off = 0x00,
		green = 0x01,
		red = 0x02,
		orange = 0x03,
		sound = 0x04
	};
	BSimaticIdent(QObject* parent = nullptr);
	BSimaticIdent(QIODevice* device, QObject* parent = nullptr);
    ~BSimaticIdent();

	bool isConnected();

signals:
	/*
	 * Wird emittiert, wenn ein Tansponder im Lesebereich gefunden wurde und ausgelesen werden konnte.
	 * Liefert die Seriennummer in Hex darstellung.
	 * Alle Buchstaben sind groß.
	 */
    void login(const QString serialnumber);
	/*
	* Wird emittiert, wenn der ausgelesene Transponder den Lesebereich verlässt
	* oder die Verbindung zum Cardreader verloren geht während sich ein ausgelesener Transponder im Lesebereich befindet.
	*/
    void logout();

public slots:
	/*
	* Schaltet die LED auf eine bestimmte Farbe.
	* In enum LEDColor sind die möglichen Farben aufgelistet.
	* Wenn die soundDuration größer 0 ist, wird als erstes der Ton ausgeben und im Anschluss die LED geschaltet.
	* Mit dieser Funktion kann auch nur der Ton von bestimmter Dauer ausgegeben werden, Bed.: color = LEDColor::off.
	* Die Einstellung wird zurückgesetzt sobald sich kein Transponder mehr im Lesebereich aufhält.
	*/
	void setLed(const char color, const unsigned int soundDurationInMS = 0);
	void setDevice(const QString comPort);
	void setDevice(QIODevice* device);

private slots:
    void checkID();


private:
    enum Command : char {
        commandWithChecksum = 0x1C,
        commandWithoutChecksum = 0x0C,
    };

    enum Response : char {
        responseWithChecksum = 0x1C,
        responseWithoutChecksum = 0x0C,
        responseWithChecksumAndStatusCode = char(0x9C),
        responseWithoutChecksumAndStatusCode = char(0x8C)
    };

    enum StatusCode : char {
        OK = 0x00,
        NOTAG_ERR = 0x01,
        CARD_NOT_SELECTED_ERR = 0x02,
        HF_ERR = 0x03,
        CONFIG_ERR = 0x04,
        AUTH_ERR = 0x05,
        READ_ERR = 0x06,
        WRITE_ERR = 0x07,
        CONFCARD_READ = 0x08,
        INVALID_CARD_FAMILY_ERR = 0x09,
        NOT_SUPPORTED_ERR = 0x0A,
        VHL_FORMAT_ERR = 0x0B,
        VHL_HW_ERR = 0x0C,

        CMD_WORK = 0x40,
        INVAILD_CMD_ERR = 0x41,
        ACCESS_DENIED_ERR = 0x42,
        TIMEOUT_ERR = char(0x81),
        FRAME_ERR = char(0x82),
        AME_OVERFLOW_ERR = char(0x83),
        CHK_ERR = char(0x84),

        UNKNOWN = char(0xFF)
    };

    //Befehle an den CardReader
    StatusCode syscmd_rest();
    StatusCode syscmd_get_info();
    StatusCode syscmd_get_boot_status();
    StatusCode syscmd_set_port(const int color);
    StatusCode vhl_select();
    StatusCode vhl_get_snr(QString* serialNumber);
    StatusCode vhl_is_selcted();
    StatusCode vhl_read();
    StatusCode vhl_write();

    //Variablen
    bool _isCardDetected = false;
    QIODevice* _serialPort = nullptr;
    int _timeoutCounter = 0;
    QTimer* _timer;

    //Eigene Funktionen
    char calculateChecksum(const QByteArray &byteArray);
    bool confirmResponse(const QByteArray &Command, const QByteArray &Response);
    QString extractSerialnumber(const QByteArray &Response);
    StatusCode extractStatusCode(const QByteArray &Response);
    bool isResponseWithStatusCode(const QByteArray &Response);
    bool verifyChecksum(const QByteArray &Response);

};

#endif // BSIMATICIDENT_H

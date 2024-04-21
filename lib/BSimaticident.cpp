/*!
 * Eine Klasse zur Herstellung einer seriellen Verbindung über RS232 mit einem Siemens Kartenlesegerät.
 *
 * Beim Erstellen eines Objekts der Klasse, öffnet die Klasse die serielle Schnittstelle.
 * Danach wird zyklisch eine Funktion ausgeführt, die nach einem Transponder sucht.
 * Wird ein Transponder gefunden, wird die Seriennummer ausgelesen und ein login mit der Seriennummer ausgeben.
 * Solange der Transponder sich im Feld befindet bleibt dieser Ausgewählt und erst nachdem er entfernt wurde wird ein Logout ausgeben, sodass sich andere Transponder anmelden können.
 * Beim Verbindungsverlust oder bei bestimmten Fehlern wird die Verbindung beendet und es wird automatisch versucht eine Verbindung wieder aufzubauen.
 *
 */

#include "BSimaticident.h"
#include <QThread>
/*
 * Konstruktor der Klasse SimaticIdent.
 *
 * Der Timer sorgt für eine zyklische Ausführung der Funktion CheckID
 *
 */
BSimaticIdent::BSimaticIdent(QObject *parent) : QObject(parent)
{

}


BSimaticIdent::BSimaticIdent(QIODevice* device, QObject *parent) : QObject(parent)
{
	if (device) {
		setDevice(device);
	} else {
		qFatal("BSimaticIdent: No device Found");
	}
}

/*
 * Destruktor der Klasse SimaticIdent.
 *
 */
BSimaticIdent::~BSimaticIdent()
{
	syscmd_set_port(off);
	delete _serialPort;
}

/*
 * Überprüft, ob eine serielle Verbindung besteht.
 *
 * \return boolean
 */
bool BSimaticIdent::isConnected()
{
    return _serialPort->isOpen();
}

void BSimaticIdent::setDevice(const QString comPort)
{
	setDevice(new QSerialPort(comPort));
}

void BSimaticIdent::setDevice(QIODevice* device)
{
	_serialPort = device;

	_timer = new QTimer();
	connect(_timer, &QTimer::timeout, this, &BSimaticIdent::checkID);
	_timer->start(100);

	syscmd_set_port(off);
}

void BSimaticIdent::setLed(const char color, const unsigned int soundDurationInMS)
{
	if (soundDurationInMS > 0) {
		syscmd_set_port(sound | static_cast<LEDColor>(color));
        std::this_thread::sleep_for(std::chrono::milliseconds(soundDurationInMS));
	}
	syscmd_set_port(static_cast<LEDColor>(color));
}

//main
void BSimaticIdent::checkID()
{
    if (isConnected()) {
        StatusCode status;

        if (_isCardDetected) {
            status = vhl_is_selcted();
            if (status != BSimaticIdent::OK) {
                _isCardDetected = false;
                syscmd_set_port(off);
                emit logout();
            }

        } else {
            status = vhl_select();

            // If no card is in field we are finished
            if (status == BSimaticIdent::NOTAG_ERR || status == BSimaticIdent::HF_ERR) {
                return;
            }

            if (status == BSimaticIdent::OK) {
                QString serialnumber;
                status = vhl_get_snr(&serialnumber);

                if (status == BSimaticIdent::OK) {
                    _isCardDetected = true;
					serialnumber = serialnumber.toUpper();
                    emit login(serialnumber);
                }
            }
        }

        // disconnect reader on error
        switch (status) {
        case FRAME_ERR:
            qDebug() << "Frame error";
            break;
        case CHK_ERR:
            qDebug() << "Checksum error";
            break;
        case TIMEOUT_ERR:
            qDebug() << "Timeout error";
            break;
        case UNKNOWN:
            syscmd_rest();
            qDebug() << "Unknown error";
			_serialPort->close();
            break;
        default:
            break;
        }

    } else {
		_serialPort->open(QIODevice::ReadWrite);
    }
}

/*
 * Berechnet aus dem gegebenen Bytearry die Prüfsumme.
 * Die Prüfsumme ergibt sich aus der XOR-Verknüfung aller Bytes, bis auf den letzten.
 *
 * \return char mit der Prüfsumme
 */
char BSimaticIdent::calculateChecksum(const QByteArray &byteArray)
{
    char checksum = 0;

    for(int i = 0; i < byteArray.size() - 1; i++) {
        checksum ^= byteArray.at(i);
    }
    return checksum;
}


/*
 * Extrahiert aus den empfangengen Bytes von der Funktion vhl_get_snr die Seriennummer.
 *
 * \return QString
 */
QString BSimaticIdent::extractSerialnumber(const QByteArray &response)
{
    const int BytePosOfSerialnumber = 5;
    const int minLenghtOfResponse = 3;

    QString serialnumber;
    if (!response.isEmpty() && minLenghtOfResponse < response.size()) {
        int serialnumberLength = response.at(3);
        for (int i = BytePosOfSerialnumber; i < (BytePosOfSerialnumber + serialnumberLength); i++) {
            serialnumber.append(QString("%1").arg(static_cast<uchar>(response.at(i)), 0, 16));
        }
        return serialnumber;
    }
    return QString();
}


BSimaticIdent::StatusCode BSimaticIdent::extractStatusCode(const QByteArray &response)
{
    if (6 <= response.size()) {
        switch (static_cast<StatusCode>(response.at(5))) {
        case OK:
        case NOTAG_ERR:
        case CARD_NOT_SELECTED_ERR:
        case HF_ERR:
        case CONFIG_ERR:
        case AUTH_ERR:
        case READ_ERR:
        case WRITE_ERR:
        case CONFCARD_READ:
        case INVALID_CARD_FAMILY_ERR:
        case NOT_SUPPORTED_ERR:
        case VHL_FORMAT_ERR:
        case VHL_HW_ERR:

        case CMD_WORK:
        case INVAILD_CMD_ERR:
        case ACCESS_DENIED_ERR:
        case TIMEOUT_ERR:
        case FRAME_ERR:
        case AME_OVERFLOW_ERR:
        case CHK_ERR:

            return static_cast<StatusCode>(response.at(5));

        default: return UNKNOWN;
        }
    }
    return UNKNOWN;
}


bool BSimaticIdent::verifyChecksum(const QByteArray &response)
{
    if (response.isEmpty()) {
        return false;
    } else if ((response.front() == responseWithoutChecksum) || (response.front() == responseWithoutChecksumAndStatusCode)) {
        return true;
    } else {
        return response.back() == calculateChecksum(response);
    }
}


bool BSimaticIdent::confirmResponse(const QByteArray &command, const QByteArray &response)
{
    const int DevCodeBytePos = 1;
    const int CmdCodeBytePos = 2;
    const int minNumberofBytes = 4;

    if (!command.isEmpty() && !response.isEmpty()) {
        if(minNumberofBytes < command.size() && minNumberofBytes < response.size()) {
            if ((command.at(DevCodeBytePos) == response.at(DevCodeBytePos)) && (command.at(CmdCodeBytePos) == response.at(CmdCodeBytePos))) {
                if ((command.front() == commandWithChecksum && (response.front() == responseWithChecksum || response.front() == responseWithChecksumAndStatusCode))
                        || (command.front() == commandWithoutChecksum && (response.front() == responseWithoutChecksum || response.front() == responseWithoutChecksumAndStatusCode))) {
                    if (verifyChecksum(response)) {
                        return true;
                    } else {
                        qWarning() << "Incorrect checksum";
                    }
                } else {
                    qWarning() << "Command and Response type don't match";
                }
            } else {
                qWarning() << "Command and Response don't match";
            }
        } else {
            qWarning() << "command or response to short";
        }
    } else {
        qWarning() << "Empty command or response";
    }
    return false;
}


bool BSimaticIdent::isResponseWithStatusCode(const QByteArray &response)
{
    if (!response.isEmpty()) {
        return response.front() == responseWithChecksumAndStatusCode || response.front() == responseWithoutChecksumAndStatusCode;
    }
    return false;
}


BSimaticIdent::StatusCode BSimaticIdent::syscmd_rest()
{
    QByteArray command = QByteArray(6, char(0));
    command[0] = commandWithChecksum;
    command[1] = 0x00;
    command[2] = 0x03;
    command[3] = 0x00;
    command[4] = 0x00;
    command[5] = calculateChecksum(command);

    if (_serialPort->write(command) == command.size()) {
        if (_serialPort->waitForReadyRead(100)) {
            QByteArray readData = _serialPort->readAll();
            if (confirmResponse(command, readData)) {
                return OK;
            } else {
                return CHK_ERR;
            }
        } else {
            return TIMEOUT_ERR;
        }
    } else {
        return FRAME_ERR;
    }
}


BSimaticIdent::StatusCode BSimaticIdent::syscmd_get_info()
{
    QByteArray command = QByteArray(6, char(0));
    command[0] = commandWithChecksum;
    command[1] = 0x00;
    command[2] = 0x04;
    command[3] = 0x00;
    command[4] = 0x00;
    command[5] = calculateChecksum(command);

    if (_serialPort->write(command) == command.size()) {
        if (_serialPort->waitForReadyRead(50)) {
            QByteArray readData = _serialPort->readAll();
            if (confirmResponse(command, readData)) {
                return OK;
            } else {
                return CHK_ERR;
            }
        } else {
            return TIMEOUT_ERR;
        }
    } else {
        return FRAME_ERR;
    }
}


BSimaticIdent::StatusCode BSimaticIdent::syscmd_get_boot_status()
{
    QByteArray command = QByteArray(6, char(0));
    command[0] = commandWithChecksum;
    command[1] = 0x00;
    command[2] = 0x05;
    command[3] = 0x00;
    command[4] = 0x00;
    command[5] = calculateChecksum(command);

    if (_serialPort->write(command) == command.size()) {
        if (_serialPort->waitForReadyRead(50)) {
            QByteArray readData = _serialPort->readAll();
            if (confirmResponse(command, readData)) {
                return OK;
            } else {
                return CHK_ERR;
            }
        } else {
            return TIMEOUT_ERR;
        }
    } else {
        return FRAME_ERR;
    }
}


BSimaticIdent::StatusCode BSimaticIdent::syscmd_set_port(const int color)
{
    QByteArray command = QByteArray(8,char(0));
    command[0] = commandWithChecksum;
    command[1] = 0x00;
    command[2] = 0x07;
    command[3] = 0x02;
    command[4] = 0x00;
    command[5] = 0x00;
    command[6] = color;
    command[7] = calculateChecksum(command);

    if (_serialPort->write(command) == command.size()) {
        if (_serialPort->waitForReadyRead(100)) {
            QByteArray readData = _serialPort->readAll();
            if (confirmResponse(command, readData)) {
                return OK;
            } else {
                return CHK_ERR;
            }
        } else {
            return TIMEOUT_ERR;
        }
    } else {
        return FRAME_ERR;
    }
}


BSimaticIdent::StatusCode BSimaticIdent::vhl_select()
{
    QByteArray command = QByteArray(10,char(0));
    command[0] = commandWithChecksum;
    command[1] = 0x01;
    command[2] = 0x00;
    command[3] = 0x04;
    command[4] = 0x00;
    command[5] = char(0xFF);
    command[6] = char(0xFF);
    command[7] = 0x01;
    command[8] = 0x00;
    command[9] = calculateChecksum(command);

    if (_serialPort->write(command) == command.size()) {
        if (_serialPort->waitForReadyRead(3000)) {
			QByteArray readData = _serialPort->readAll();
            if (confirmResponse(command, readData)) {
                if (isResponseWithStatusCode(readData)) {
                    return extractStatusCode(readData);
                } else {
                    return OK;
                }
            } else {
                return CHK_ERR;
            }
        } else {
			 return TIMEOUT_ERR;
        }
    } else {
        return FRAME_ERR;
    }
}


BSimaticIdent::StatusCode BSimaticIdent::vhl_get_snr(QString* serialNumber)
{
    QByteArray command = QByteArray(6, char(0));
    command[0] = commandWithChecksum;
    command[1] = 0x01;
    command[2] = 0x01;
    command[3] = 0x00;
    command[4] = 0x00;
    command[5] = calculateChecksum(command);

    if (_serialPort->write(command) == command.size()) {
        if (_serialPort->waitForReadyRead(100)) {
            QByteArray readData = _serialPort->readAll();
            if (confirmResponse(command, readData)) {
                if (isResponseWithStatusCode(readData)) {
                    return extractStatusCode(readData);
                } else {
                    if (serialNumber) {
                        *serialNumber = extractSerialnumber(readData);
                    }
                    return OK;
                }
            } else {
                return CHK_ERR;
            }
        } else {
            return TIMEOUT_ERR;
        }
    } else {
        return FRAME_ERR;
    }
}


BSimaticIdent::StatusCode BSimaticIdent::vhl_is_selcted()
{
    QByteArray command = QByteArray(6, char(0));
    command[0] = commandWithChecksum;
    command[1] = 0x01;
    command[2] = 0x04;
    command[3] = 0x00;
    command[4] = 0x00;
    command[5] = calculateChecksum(command);

    if (_serialPort->write(command) == command.size()) {
        if (_serialPort->waitForReadyRead(3000)) {
            QByteArray readData = _serialPort->readAll();
            if (confirmResponse(command, readData)) {
                if (isResponseWithStatusCode(readData)) {
                    return extractStatusCode(readData);
                } else {
                    return OK;
                }
            } else {
                return CHK_ERR;
            }
        } else {
            return TIMEOUT_ERR;
        }
    } else {
        return FRAME_ERR;
    }
}

BSimaticIdent::StatusCode BSimaticIdent::vhl_read()
{
    QByteArray command = QByteArray(11, char(0));
    command[0] = commandWithChecksum;
    command[1] = 0x01;
    command[2] = 0x02;
    command[3] = 0x05;
    command[4] = 0x00;
    command[5] = 0x01;
    command[6] = 0x00;
    command[7] = 0x37;
    command[8] = 0x00;
    command[9] = 0x08;
    command[10] = calculateChecksum(command);

    if (_serialPort->write(command) == command.size()) {
        if (_serialPort->waitForReadyRead(3000)) {
            QByteArray readData = _serialPort->readAll();
            if (confirmResponse(command, readData)) {
                if (isResponseWithStatusCode(readData)) {
                    return extractStatusCode(readData);
                } else {
                    return OK;
                }
            } else {
                return CHK_ERR;
            }
        } else {
            return TIMEOUT_ERR;
        }
    } else {
        return FRAME_ERR;
    }
}


BSimaticIdent::StatusCode BSimaticIdent::vhl_write()
{
    QByteArray command = QByteArray(11, char(0));
    command[0] = commandWithChecksum;
    command[1] = 0x01;
    command[2] = 0x03;
    command[3] = 0x0D;
    command[4] = 0x00;
    command[5] = 0x01;
    command[6] = 0x00;
    command[7] = 0x42;
    command[8] = 0x00;
    command[9] = 0x08;
    command[10] = calculateChecksum(command);

    if (_serialPort->write(command) == command.size()) {
        if (_serialPort->waitForReadyRead(3000)) {
            QByteArray readData = _serialPort->readAll();
            if (confirmResponse(command, readData)) {
                if (isResponseWithStatusCode(readData)) {
                    return extractStatusCode(readData);
                } else {
                    return OK;
                }
            } else {
                return CHK_ERR;
            }
        } else {
            return TIMEOUT_ERR;
        }
    } else {
        return FRAME_ERR;
    }
}

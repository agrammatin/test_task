/*
Тестовое задание

Описание протокола:

Преамбула 4 байта 0x01 0x02 0x03 0x04
Длина сообщения 1 байт
Адрес источника 1 байт
Адрес приёмника 1 байт
Данные N байт
CRC - XOR всех байт сообщения включая преамбулу

максимальное время приёма сообщения 1 сек.



Задание 1.
Написать прототипы функций которые потребуются для обработки протокола.

Задание 2.
Представить реализацию на C алгоритма приёма сообщений протокола.
*/

#include <stdlib.h>
#include <stdio.h>

typedef unsigned char	u8_t;

const u8_t MAX_LEN 		= 255;
const u8_t MIN_LEN 		= 9;
const u8_t PRIAM_1 		= 0x01;
const u8_t PRIAM_2 		= 0x02;
const u8_t PRIAM_3		= 0x03;
const u8_t PRIAM_4 		= 0x04;
const u8_t ADR_SOURCE 	= 0xAA;
const u8_t ADR_RECEIV 	= 0x55;

struct Message_st
{
    u8_t length_mes;
    u8_t adr_source_mes;
    u8_t data_mes[MAX_LEN-8];
};


// Задание 1.

// Получение посылки 
u8_t get_post(u8_t* buf, u8_t lenth_buf, u8_t* receiver);

// Проверки посылки
u8_t not_check_post(u8_t* post, u8_t lenth);

// Получение даных
struct Message_st* read_post(struct Message_st* mess_strk, u8_t* buf_mess);

// Обработка даных
void print_mes(struct Message_st* mess_strk);

// Очистка буфера
void clear_buf(u8_t* buf);


// Задание 2.

int main()
{
	u8_t* buffer = NULL;
	u8_t lenth_buffer = 0;
	u8_t* rec_buf = NULL;
	struct Message_st* message_st = (struct Message_st*)malloc(sizeof(struct Message_st));
    while (1) {
		// Получение посылки - заполнение buffer с подсчётом принятых байт lenth_buffer.
		// Перед записью очистка buffer, обнуление счётчика
		// Для микроконтроллера заполнение в прирывании, необходим сигнал синхронизации.
		// Для контроля максимального времени приёма необходимо включение таймера в момент
		//	начала приёма и контроль времени по окончании передачи.
		
		
		u8_t len_post = get_post(buffer, lenth_buffer, rec_buf);
		if (!len_post) {
			continue;
		}
		if (!not_check_post(rec_buf, len_post)){
			read_post(message_st, rec_buf);
			print_mes(message_st);
		}	
		clear_buf(rec_buf);
		// возможен break в зависимости от плученных даных
    };
	clear_buf(buffer);
	free(message_st);
	message_st = NULL;
    return 0;
}

// Получение посылки 
u8_t get_post(u8_t* buf, u8_t lenth_buf, u8_t* receiver)
{
	if((buf != NULL) && (lenth_buf >= MIN_LEN)) {
		if (lenth_buf > MAX_LEN) {
			lenth_buf = MAX_LEN;
		}
		receiver = (u8_t*) malloc(lenth_buf);
		if (receiver != NULL) {
			for (u8_t i=0; i<lenth_buf; i++) {
				*(receiver+i) = *(buf);
			}
		}
		return lenth_buf;
	} else {
		return 0;
	}
}

// Проверки посылки
u8_t not_check_post(u8_t* post, u8_t lenth) 
{	
	// Проверка приамбулы
    if ((*post!=PRIAM_1) || (*(post+1)!=PRIAM_2) ||
            (*(post+2)!=PRIAM_3) || (*(post+3)!=PRIAM_4)) {
        return 1;
    }
	
	// Проверка длины
    if (lenth < *(post+4)) {
        return 2;
    }

    // Проверка CRC
    u8_t len = *(post+4);
    u8_t crc_post = *(post+len-1);
    u8_t crc_real = *post;
    for (u8_t i=1; i<(len-1); i++) {
        crc_real ^= *(post+i);
    }
    if (crc_real != crc_post) {
        return 3;
    }

/*
    // Проверка адреса отправителя
    if (*(post+5) != ADR_SOURCE) {
        return 4;
    }
*/

    // Проверка адреса получателя
    if (*(post+6) != ADR_RECEIV) {
        return 5;
    }
    return 0;
}

// Получение даных
struct Message_st* read_post(struct Message_st* mess_struck, u8_t* buf_mess)
{
	mess_struck->length_mes = *(buf_mess+4) - 8;
	mess_struck->adr_source_mes = *(buf_mess+5);
	for (int i=0; i<mess_struck->length_mes; i++) {
		mess_struck->data_mes[i] = *(buf_mess+7+i);
	}
    return mess_struck;
}

// Обработка даных
void print_mes(struct Message_st* mess_struck) {
	printf("Adress source %u \n", mess_struck->adr_source_mes);
    for (int i=0; i<(mess_struck->length_mes); i++) {
        printf("%u ", *(mess_struck->data_mes + i));
    }
}

// Очистка буфера
void clear_buf(u8_t* buf) {
	free(buf);
	buf = NULL;
}

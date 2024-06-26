#include "iarduino_PCA9555.h"																									//
																																//
//		Инициализация модуля:																									//	Возвращаемое значение: результат инициализации.
bool	iarduino_PCA9555::_begin		(void){																					//	Параметр: отсутствует
		//	Проверяем наличие чипа на шине I2C:																					//
			uint8_t start = valAddrTemp; if( start==0 ){ start=0x20; }															//	Первый    проверяемый адрес на шине I2C.
			uint8_t stop  = valAddrTemp; if( stop ==0 ){ stop =0x27; }															//	Последний проверяемый адрес на шине I2C.
			for(uint8_t i=start; i<=stop; i++){ if( selI2C->checkAddress(i) ){ valAddr=i; i=stop+1; } }							//	Если на шине I2C есть устройство с адресом от start до stop, то сохраняем его адрес в «valAddr».
		//	Если модуль не найден, то возвращаем ошибку инициализации:															//
			if( valAddr==0 ){ return false; }																					//
			delay(5);																											//
		//	Перезагружаем модуль устанавливая его регистры в значение по умолчанию:												//
			reset();																											//	Выполняем программную перезагрузку.
			delay(5);																											//
			return true;																										//	Возвращаем флаг успешной инициализаии.
}																																//
																																//
//		Сброс модуля:																											//	Возвращаемое значение:	результат перезагрузки.
bool	iarduino_PCA9555::reset			(void){																					//	Параметр:				отсутствует.
			if(valAddr){																										//	Если модуль был инициализирован, то ...
			//	Конфигурируем все выводы как входы:																				//
				if( portWrite(2, 0x0000)==false ){ return false; }																//	Устанавливаем уровень логического 0 на всех выводах обоих портов.
				if( portMode (2, 0xFFFF)==false ){ return false; }																//	Конфигурируем выводы обоих портов в режим входов.
				return true;																									//
			}else{																												//	Иначе, если модуль не инициализирован, то ...
				return false;																									//	Возвращаем ошибку.
			}																													//
}																																//
																																//
//		Конфигурирование одного вывода:																							//	Возвращаемое значение:	резульат конфигурирования.
bool	iarduino_PCA9555::pinMode		(uint8_t pin, uint8_t dir){																//	Параметр:				pin - номер вывода (0...15), dir - направление (INPUT/OUTPUT).
			if(valAddr){																										//	Если модуль был инициализирован, то ...
			//	Проверяем введённые данные:																						//
				if( pin>15 ){ return false; }																					//	Номер вывода, это беззнаковое целое не больше 15.
				if( dir==INPUT  ){ dir=1; }else if( dir==OUTPUT ){ dir=0; }else{ return false; }								//	Направление работы вывода может принимать значение INPUT или OUTPUT.
			//	Меняем состояние бита в байте конфигурации:																		//
				bitWrite( valCONFIG[pin/8], pin%8, dir );																		//
			//	Отправляем байт конфигурации в регистр конфигурации:															//
				data[0] = valCONFIG[pin/8];																						//	Определяем значение для передачи.
				if(_writeBytes( REG_PCA9555_CONFIG+(pin/8), 1 )==false){ return false; }										//	Отправляем 1 байт из массива «data» в регистр REG_PCA9555_CONFIG+(pin/8).
				delay(1);																										//	Даём более чем достаточное время для применения модулем нового адреса.
				return true;																									//	Возвращаем флаг успеха.
			}else{																												//	Иначе, если модуль не инициализирован, то ...
				return false;																									//	Возвращаем ошибку.
			}																													//
}																																//
																																//
//		Чтение уровня с одного вывода:																							//	Возвращаемое значение:	логический уровень.
bool	iarduino_PCA9555::digitalRead		(uint8_t pin){																		//	Параметр:				pin - номер вывода (0...15).
			if(valAddr){																										//	Если модуль был инициализирован, то ...
			//	Проверяем введённые данные:																						//
				if( pin>15 ){ return false; }																					//	Номер вывода, это беззнаковое целое не больше 15.
			//	Читаем байт из входного регистра:																				//
				if(_readBytes( REG_PCA9555_INPUT+(pin/8), 1 )==false){ return false; }											//	Читаем 1 байт из регистра REG_PCA9555_INPUT+(pin/8) в массив «data».
				delay(1);																										//	Даём более чем достаточное время для применения модулем нового адреса.
				return bitRead( data[0], pin%8 );																				//	Возвращаем бит под номером pin%8 из байта data[0].
			}else{																												//	Иначе, если модуль не инициализирован, то ...
				return false;																									//	Возвращаем ошибку.
			}																													//
}																																//
																																//
//		Установка уровня на одном выводе:																						//	Возвращаемое значение:	резульат конфигурирования.
bool	iarduino_PCA9555::digitalWrite		(uint8_t pin, bool val){															//	Параметр:				pin - номер вывода (0...15), val - логический уровень.
			if(valAddr){																										//	Если модуль был инициализирован, то ...
			//	Проверяем введённые данные:																						//
				if( pin>15 ){ return false; }																					//	Номер вывода, это беззнаковое целое не больше 15.
			//	Меняем состояние бита в байте устанавливаемых значений:															//
				bitWrite( valOUTPUT[pin/8], pin%8, val );																		//
			//	Отправляем байт устанавливаемых значений в выходной регистр:													//
				data[0] = valOUTPUT[pin/8];																						//	Определяем значение для передачи.
				if(_writeBytes( REG_PCA9555_OUTPUT+(pin/8), 1 )==false){ return false; }										//	Отправляем 1 байт из массива «data» в регистр REG_PCA9555_OUTPUT+(pin/8).
				delay(1);																										//	Даём более чем достаточное время для применения модулем нового адреса.
				return true;																									//	Возвращаем флаг успеха.
			}else{																												//	Иначе, если модуль не инициализирован, то ...
				return false;																									//	Возвращаем ошибку.
			}																													//
}																																//
																																//
//		Конфигурирование выводов порта:																							//	Возвращаемое значение:	резульат конфигурирования.
bool	iarduino_PCA9555::portMode			(uint8_t port, uint16_t	dir){														//	Параметры:				port - номер порта (0/1/2), dir - направления (один или два байта битов).
			if(valAddr){																										//	Если модуль был инициализирован, то ...
			//	Проверяем введённые данные:																						//
				if( port>2 ){ return false; }																					//	Номер порта, это беззнаковое целое не больше 2.
				dir=~dir;																										//	Инвертитуем указанные биты. Константа OUTPUT=1, а регистре REG_PCA9555_CONFIG биты output=0.
			//	Меняем значение байта конфигурации:																				//
				if( port<2 ){ valCONFIG[port]=dir; }																			//	Если указан порт 0 или 1, то меняем байт valCONFIG[port]
				else		{ valCONFIG[0]=lowByte(dir); valCONFIG[1]=highByte(dir); }											//	Если указан порт 2, то меняем два байта valCONFIG[]
			//	Отправляем байт конфигурации в регистр конфигурации:															//
				if( port<2 ){ data[0] = valCONFIG[port]; }																		//
				else		{ data[0] = valCONFIG[0]; data[1] = valCONFIG[1]; }													//
				if(_writeBytes( REG_PCA9555_CONFIG+(port==1?1:0), port<2?1:2 )==false){ return false; }							//	Отправляем один или два байта из массива «data» в регистр REG_PCA9555_CONFIG+(port==1?1:0).
				delay(1);																										//	Даём более чем достаточное время для применения модулем нового адреса.
				return true;																									//	Возвращаем флаг успеха.
			}else{																												//	Иначе, если модуль не инициализирован, то ...
				return false;																									//	Возвращаем ошибку.
			}																													//
}																																//
																																//
//		Чтение уровней со всех выводов порта:																					//	Возвращаемое значение:	один или два байта битов.
uint16_t iarduino_PCA9555::portRead			(uint8_t port){																		//	Параметр:				port - номер порта (0/1/2).
			if(valAddr){																										//	Если модуль был инициализирован, то ...
			//	Проверяем введённые данные:																						//
				if( port>2 ){ return false; }																					//	Номер порта, это беззнаковое целое не больше 2.
			//	Читаем байт из входного регистра:																				//
				if(_readBytes( REG_PCA9555_INPUT+(port==1?1:0), port<2?1:2 )==false){ return false; }							//	Читаем один или два байта из регистра REG_PCA9555_INPUT+(port==1?1:0) в массив «data».
				delay(1);																										//	Даём более чем достаточное время для применения модулем нового адреса.
				return port<2? data[0] : (data[1]<<8)|data[0];																	//	Возвращаем один или два байта из массива data.
			}else{																												//	Иначе, если модуль не инициализирован, то ...
				return false;																									//	Возвращаем ошибку.
			}																													//
}																																//
																																//
//		Установка уровней на всех выводах порта:																				//	Возвращаемое значение:	резульат конфигурирования.
bool	iarduino_PCA9555::portWrite			(uint8_t port, uint16_t	val){														//	Параметры:				port - номер порта (0/1/2), val - логические уровени (один или два байта битов).
			if(valAddr){																										//	Если модуль был инициализирован, то ...
			//	Проверяем введённые данные:																						//
				if( port>2 ){ return false; }																					//	Номер порта, это беззнаковое целое не больше 2.
			//	Меняем состояние байта устанавливаемых значений:																//
				if( port<2 ){ valOUTPUT[port]=val; }																			//	Если указан порт 0 или 1, то меняем байт valOUTPUT[port]
				else		{ valOUTPUT[0]=lowByte(val); valOUTPUT[1]=highByte(val); }											//	Если указан порт 2, то меняем два байта valOUTPUT[]
			//	Отправляем байт устанавливаемых значений в выходной регистр:													//
				if( port<2 ){ data[0] = valOUTPUT[port]; }																		//
				else		{ data[0] = valOUTPUT[0]; data[1] = valOUTPUT[1]; }													//
				if(_writeBytes( REG_PCA9555_OUTPUT+(port==1?1:0), port<2?1:2 )==false){ return false; }							//	Отправляем один или два байта из массива «data» в регистр REG_PCA9555_OUTPUT+(port==1?1:0).
				delay(1);																										//	Даём более чем достаточное время для применения модулем нового адреса.
				return true;																									//	Возвращаем флаг успеха.
			}else{																												//	Иначе, если модуль не инициализирован, то ...
				return false;																									//	Возвращаем ошибку.
			}																													//
}																																//
																																//
//		Чтение данных из регистров в массив data:																				//	Возвращаемое значение:	результат чтения (true/false).
bool	iarduino_PCA9555::_readBytes		(uint8_t reg, uint8_t sum){															//	Параметры:				reg - номер первого регистра, sum - количество читаемых байт.
			bool	result=false;																								//	Определяем флаг       для хранения результата чтения.
			uint8_t	sumtry=10;																									//	Определяем переменную для подсчёта количества оставшихся попыток чтения.
			do{	result = selI2C->readBytes(valAddr, reg, data, sum);															//	Считываем из модуля valAddr, начиная с регистра reg, в массив data, sum байт.
				sumtry--;	if(!result){delay(1);}																				//	Уменьшаем количество попыток чтения и устанавливаем задержку при неудаче.
			}	while		(!result && sumtry>0);																				//	Повторяем чтение если оно завершилось неудачей, но не более sumtry попыток.
			delayMicroseconds(500);																								//	Между пакетами необходимо выдерживать паузу.
			return result;																										//	Возвращаем результат чтения (true/false).
}																																//
																																//
//		Запись данных в регистры из массива data:																				//	Возвращаемое значение:	результат записи (true/false).
bool	iarduino_PCA9555::_writeBytes		(uint8_t reg, uint8_t sum, uint8_t num){											//	Параметры:				reg - номер первого регистра, sum - количество записываемых байт, num - номер первого элемента массива data.
			bool	result=false;																								//	Определяем флаг       для хранения результата записи.
			uint8_t	sumtry=10;																									//	Определяем переменную для подсчёта количества оставшихся попыток записи.
			do{	result = selI2C->writeBytes(valAddr, reg, &data[num], sum);														//	Записываем в модуль valAddr начиная с регистра reg, sum байи из массива data начиная с элемента num.
				sumtry--;	if(!result){delay(1);}																				//	Уменьшаем количество попыток записи и устанавливаем задержку при неудаче.
			}	while		(!result && sumtry>0);																				//	Повторяем запись если она завершилась неудачей, но не более sumtry попыток.
			delay(10);																											//	Ждём применения модулем записанных данных.
			return result;																										//	Возвращаем результат записи (true/false).
}																																//
																																//
#include "../../include/models/archivoHeader.hpp"

ArchivoHeader::ArchivoHeader(int cantRgt, int proxId, int rgtAct, int ver) : cantidadRegistros(cantRgt), proximoId(proxId), registrosActivos(rgtAct), version(ver) {}

int ArchivoHeader::getCantidadRegistros() { return cantidadRegistros; }

int ArchivoHeader::getProximoId() { return proximoId; }

int ArchivoHeader::getRegistrosActivos() { return registrosActivos; }

int ArchivoHeader::getVersion() { return version; }

size_t ArchivoHeader::getTamanoArchivoHeader() { return sizeof(ArchivoHeader); }

void ArchivoHeader::aumentarCantidadDeRegistros() { cantidadRegistros++; }

void ArchivoHeader::aumentarProximoId() { proximoId++; }

void ArchivoHeader::aumentarRegistrosActivos() { registrosActivos++; }

void ArchivoHeader::disminuirRegistrosActivos() { registrosActivos--; }

void ArchivoHeader::aumentarVersion() { version++; }
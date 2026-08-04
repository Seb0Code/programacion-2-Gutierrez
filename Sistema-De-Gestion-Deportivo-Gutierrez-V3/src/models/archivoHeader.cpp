#include "../../include/models/archivoHeader.hpp"

ArchivoHeader::ArchivoHeader() : ArchivoHeader(0, 1, 0, 1) {}

ArchivoHeader::ArchivoHeader(int cantRgt, int proxId, int rgtAct, int ver) : cantidadRegistros(cantRgt), proximoId(proxId), registrosActivos(rgtAct), version(ver) {}

int ArchivoHeader::getCantidadRegistros() const { return cantidadRegistros; }

int ArchivoHeader::getProximoId() const { return proximoId; }

int ArchivoHeader::getRegistrosActivos() const { return registrosActivos; }

int ArchivoHeader::getVersion() const { return version; }

size_t ArchivoHeader::getTamano() { return sizeof(ArchivoHeader); }

void ArchivoHeader::aumentarCantidadDeRegistros() { cantidadRegistros++; }

void ArchivoHeader::aumentarProximoId() { proximoId++; }

void ArchivoHeader::aumentarRegistrosActivos() { registrosActivos++; }

void ArchivoHeader::disminuirRegistrosActivos() { registrosActivos--; }

void ArchivoHeader::aumentarVersion() { version++; }

#ifndef YADE_OPENMP
#include <core/ForceContainer.hpp>
ForceContainer::ForceContainer() {};
void ForceContainer::ensureSize(Body::id_t id) {
  const Body::id_t idMaxTmp = max(id, _maxId);
  _maxId = 0;
  if(size<=(size_t)idMaxTmp) {
    resize(min((size_t)1.5*(idMaxTmp+100),(size_t)(idMaxTmp+2000)));
  };
}

const Vector3r& ForceContainer::getForce(Body::id_t id) {
  ensureSize(id);
  return _force[id];
}

void ForceContainer::addForce(Body::id_t id,const Vector3r& f) {
  ensureSize(id);
  _force[id]+=f;
}

const Vector3r& ForceContainer::getTorque(Body::id_t id) {
  ensureSize(id);
  return _torque[id];
}

void ForceContainer::addTorque(Body::id_t id,const Vector3r& t) {
  ensureSize(id);
  _torque[id]+=t;
}

const Vector3r& ForceContainer::getMove(Body::id_t id) {
  ensureSize(id);
  return _move[id];
}

void ForceContainer::addMove(Body::id_t id,const Vector3r& f) {
  ensureSize(id);
  moveRotUsed=true;
  _move[id]+=f;
}

const Vector3r& ForceContainer::getRot(Body::id_t id) {
  ensureSize(id);
  return _rot[id];
}

void ForceContainer::addRot(Body::id_t id,const Vector3r& f) {
  ensureSize(id);
  moveRotUsed=true;
  _rot[id]+=f;
}

void ForceContainer::addMaxId(Body::id_t id) {
  _maxId=id;
}

void ForceContainer::setPermForce(Body::id_t id, const Vector3r& f) {
  ensureSize(id);
  _permForce[id]=f;
  permForceUsed=true;
}

void ForceContainer::setPermTorque(Body::id_t id, const Vector3r& t) {
  ensureSize(id);
  _permTorque[id]=t;
  permForceUsed=true;
}

const Vector3r& ForceContainer::getPermForce(Body::id_t id) {
  ensureSize(id);
  return _permForce[id];
}

const Vector3r& ForceContainer::getPermTorque(Body::id_t id) {
  ensureSize(id);
  return _permTorque[id];
}

const Vector3r& ForceContainer::getForceUnsynced (Body::id_t id) {
  return getForce(id);
}

const Vector3r& ForceContainer::getTorqueUnsynced(Body::id_t id) {
  return getForce(id);
}

const Vector3r ForceContainer::getForceSingle (Body::id_t id) { 
  ensureSize(id); 
  if (permForceUsed) {
    return _force [id] + _permForce[id];
  } else {
    return _force [id];
  }
}
const Vector3r ForceContainer::getTorqueSingle(Body::id_t id) { 
  ensureSize(id); 
  if (permForceUsed) {
    return _torque[id] + _permTorque[id];
  } else {
    return _torque[id];
  }
}
const Vector3r ForceContainer::getMoveSingle(Body::id_t id) {
  ensureSize(id);
  return _move [id];
}

const Vector3r ForceContainer::getRotSingle(Body::id_t id) {
  ensureSize(id);
  return _rot[id];
}

void ForceContainer::sync() {
  if (_maxId>0) {
    ensureSize(_maxId);
    _maxId=0;
  }
  if (permForceUsed) {
    for(long id=0; id<(long)size; id++) {
      _force[id]+=_permForce[id];
      _torque[id]+=_permTorque[id];
    }
  }
  return;
}

void ForceContainer::reset(long iter, bool resetAll) {
  memset(&_force [0],0,sizeof(Vector3r)*size);
  memset(&_torque[0],0,sizeof(Vector3r)*size);
  if(moveRotUsed){
    memset(&_move  [0],0,sizeof(Vector3r)*size);
    memset(&_rot   [0],0,sizeof(Vector3r)*size);
    moveRotUsed=false;
  }
  if (resetAll){
    memset(&_permForce [0], 0,sizeof(Vector3r)*size);
    memset(&_permTorque[0], 0,sizeof(Vector3r)*size);
    permForceUsed = false;
  }
  lastReset=iter;
}

void ForceContainer::resize(size_t newSize) {
  _force.resize(newSize,Vector3r::Zero());
  _torque.resize(newSize,Vector3r::Zero());
  _permForce.resize(newSize,Vector3r::Zero());
  _permTorque.resize(newSize,Vector3r::Zero());
  _move.resize(newSize,Vector3r::Zero());
  _rot.resize(newSize,Vector3r::Zero());
  size=newSize;
}

const int ForceContainer::getNumAllocatedThreads() const {return 1;}
const bool ForceContainer::getMoveRotUsed() const {return moveRotUsed;}
const bool ForceContainer::getPermForceUsed() const {return permForceUsed;}
#endif

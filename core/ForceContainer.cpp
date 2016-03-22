#include <core/ForceContainer.hpp>

#ifdef YADE_OPENMP
#include <omp.h>
inline void ForceContainer::ensureSize(Body::id_t id, int threadN) {
  assert(nThreads>omp_get_thread_num());
  const Body::id_t idMaxTmp = max(id, _maxId[threadN]);
  _maxId[threadN] = 0;
  if (threadN<0) {
    resizePerm(min((size_t)1.5*(idMaxTmp+100),(size_t)(idMaxTmp+2000)));
  } else if (sizeOfThreads[threadN]<=(size_t)idMaxTmp) {
    resize(min((size_t)1.5*(idMaxTmp+100),(size_t)(idMaxTmp+2000)),threadN);
  }
}

inline void ForceContainer::ensureSynced() {
  if(!synced) throw runtime_error("ForceContainer not thread-synchronized; call sync() first!");
}

ForceContainer::ForceContainer() {
  nThreads=omp_get_max_threads();
  for(int i=0; i<nThreads; i++){
    _forceData.push_back(vvector()); _torqueData.push_back(vvector());
    _moveData.push_back(vvector());  _rotData.push_back(vvector());
    sizeOfThreads.push_back(0);
    _maxId.push_back(0);
  }
}

const Vector3r& ForceContainer::getForce(Body::id_t id) {
  ensureSynced();
  return ((size_t)id<size)?_force[id]:_zero;
}

void ForceContainer::addForce(Body::id_t id, const Vector3r& f){
  ensureSize(id,omp_get_thread_num());
  synced=false;
  _forceData[omp_get_thread_num()][id]+=f;
}

const Vector3r& ForceContainer::getTorque(Body::id_t id) {
  ensureSynced();
  return ((size_t)id<size)?_torque[id]:_zero;
}

void ForceContainer::addTorque(Body::id_t id, const Vector3r& t) {
  ensureSize(id,omp_get_thread_num());
  synced=false;
  _torqueData[omp_get_thread_num()][id]+=t;
}

const Vector3r& ForceContainer::getMove(Body::id_t id) {
  ensureSynced();
  return ((size_t)id<size)?_move[id]:_zero;
}

void ForceContainer::addMove(Body::id_t id, const Vector3r& m) {
  ensureSize(id,omp_get_thread_num());
  synced=false;
  moveRotUsed=true;
  _moveData[omp_get_thread_num()][id]+=m;
}

const Vector3r& ForceContainer::getRot(Body::id_t id) {
  ensureSynced();
  return ((size_t)id<size)?_rot[id]:_zero;
}

void ForceContainer::addRot(Body::id_t id, const Vector3r& r) {
  ensureSize(id,omp_get_thread_num());
  synced=false;
  moveRotUsed=true;
  _rotData[omp_get_thread_num()][id]+=r;
}

void ForceContainer::addMaxId(Body::id_t id) {
  _maxId[omp_get_thread_num()]=id;
}

void ForceContainer::setPermForce(Body::id_t id, const Vector3r& f) {
  ensureSize(id,-1);
  synced=false;
  _permForce[id]=f;
  permForceUsed=true;
}

void ForceContainer::setPermTorque(Body::id_t id, const Vector3r& t) {
  ensureSize(id,-1);
  synced=false;
  _permTorque[id]=t;
  permForceUsed=true;
}

const Vector3r& ForceContainer::getPermForce(Body::id_t id) {
  ensureSynced();
  return ((size_t)id<size)?_permForce[id]:_zero;
}

const Vector3r& ForceContainer::getPermTorque(Body::id_t id) {
  ensureSynced();
  return ((size_t)id<size)?_permTorque[id]:_zero;
}

const Vector3r& ForceContainer::getForceUnsynced(Body::id_t id) {
  assert ((size_t)id<size);
  return _force[id];
}

const Vector3r& ForceContainer::getTorqueUnsynced(Body::id_t id) {
  assert ((size_t)id<size);
  return _torque[id];
}

void ForceContainer::addForceUnsynced(Body::id_t id, const Vector3r& f) {
  assert ((size_t)id<size);
  _force[id]+=f;
}

void ForceContainer::addTorqueUnsynced(Body::id_t id, const Vector3r& m) {
  assert ((size_t)id<size);
  _torque[id]+=m;
}

Vector3r ForceContainer::getForceSingle(Body::id_t id) {
  Vector3r ret(Vector3r::Zero());
  for(int t=0; t<nThreads; t++) {
    ret+=((size_t)id<sizeOfThreads[t])?_forceData [t][id]:_zero;
  }
  if (permForceUsed) ret+=_permForce[id];
  return ret;
}

Vector3r ForceContainer::getTorqueSingle(Body::id_t id) {
  Vector3r ret(Vector3r::Zero());
  for(int t=0; t<nThreads; t++) {
    ret+=((size_t)id<sizeOfThreads[t])?_torqueData[t][id]:_zero;
  }
  if (permForceUsed) ret+=_permTorque[id];
  return ret;
}

Vector3r ForceContainer::getMoveSingle(Body::id_t id) {
  Vector3r ret(Vector3r::Zero());
  for(int t=0; t<nThreads; t++) {
    ret+=((size_t)id<sizeOfThreads[t])?_moveData[t][id]:_zero;
  }
  return ret;
}

Vector3r ForceContainer::getRotSingle(Body::id_t id) {
  Vector3r ret(Vector3r::Zero());
  for(int t=0; t<nThreads; t++) {
    ret+=((size_t)id<sizeOfThreads[t])?_rotData[t][id]:_zero;
  }
  return ret;
}

void ForceContainer::syncSizesOfContainers() {
  if (syncedSizes) return;
  //check whether all containers have equal length, and if not resize it
  for(int i=0; i<nThreads; i++){
    if (sizeOfThreads[i]<size) resize(size,i);
  }
  _force.resize(size,Vector3r::Zero());
  _torque.resize(size,Vector3r::Zero());
  _permForce.resize(size,Vector3r::Zero());
  _permTorque.resize(size,Vector3r::Zero());
  _move.resize(size,Vector3r::Zero());
  _rot.resize(size,Vector3r::Zero());
  syncedSizes=true;
}

void ForceContainer::sync(){
  for(int i=0; i<nThreads; i++){
    if (_maxId[i] > 0) { synced = false;}
  }
  if(synced) return;
  boost::mutex::scoped_lock lock(globalMutex);
  if(synced) return; // if synced meanwhile
  
  for(int i=0; i<nThreads; i++){
    if (_maxId[i] > 0) { ensureSize(_maxId[i],i);}
  }
  
  syncSizesOfContainers();

  for(long id=0; id<(long)size; id++){
    Vector3r sumF(Vector3r::Zero()), sumT(Vector3r::Zero());
    for(int thread=0; thread<nThreads; thread++){ sumF+=_forceData[thread][id]; sumT+=_torqueData[thread][id];}
    _force[id]=sumF; _torque[id]=sumT;
    if (permForceUsed) {_force[id]+=_permForce[id]; _torque[id]+=_permTorque[id];}
  }
  if(moveRotUsed){
    for(long id=0; id<(long)size; id++){
      Vector3r sumM(Vector3r::Zero()), sumR(Vector3r::Zero());
      for(int thread=0; thread<nThreads; thread++){ sumM+=_moveData[thread][id]; sumR+=_rotData[thread][id];}
      _move[id]=sumM; _rot[id]=sumR;
    }
  }
  synced=true; syncCount++;
}

void ForceContainer::resize(size_t newSize, int threadN) {
  _forceData [threadN].resize(newSize,Vector3r::Zero());
  _torqueData[threadN].resize(newSize,Vector3r::Zero());
  _moveData[threadN].resize(newSize,Vector3r::Zero());
  _rotData[threadN].resize(newSize,Vector3r::Zero());
  sizeOfThreads[threadN] = newSize;
  if (size<newSize) size=newSize;
  syncedSizes=false;
}

void ForceContainer::resizePerm(size_t newSize) {
  _permForce.resize(newSize,Vector3r::Zero());
  _permTorque.resize(newSize,Vector3r::Zero());
  if (size<newSize) size=newSize;
  syncedSizes=false;
}

void ForceContainer::reset(long iter, bool resetAll) {
  syncSizesOfContainers();
  for(int thread=0; thread<nThreads; thread++){
    memset(&_forceData [thread][0],0,sizeof(Vector3r)*sizeOfThreads[thread]);
    memset(&_torqueData[thread][0],0,sizeof(Vector3r)*sizeOfThreads[thread]);
    if(moveRotUsed){
      memset(&_moveData  [thread][0],0,sizeof(Vector3r)*sizeOfThreads[thread]);
      memset(&_rotData   [thread][0],0,sizeof(Vector3r)*sizeOfThreads[thread]);
    }
  }
  memset(&_force [0], 0,sizeof(Vector3r)*size);
  memset(&_torque[0], 0,sizeof(Vector3r)*size);
  if(moveRotUsed){
    memset(&_move  [0], 0,sizeof(Vector3r)*size);
    memset(&_rot   [0], 0,sizeof(Vector3r)*size);
  }
  if (resetAll){
    memset(&_permForce [0], 0,sizeof(Vector3r)*size);
    memset(&_permTorque[0], 0,sizeof(Vector3r)*size);
    permForceUsed = false;
  }
  if (!permForceUsed) synced=true; else synced=false;
  moveRotUsed=false;
  lastReset=iter;
}

const int& ForceContainer::getNumAllocatedThreads() {
  return nThreads;
}

const bool& ForceContainer::getMoveRotUsed() {
  return moveRotUsed;
}

const bool& ForceContainer::getPermForceUsed() {
  return permForceUsed;
}

#else
void ForceContainer::ensureSize(Body::id_t id) {
  const Body::id_t idMaxTmp = max(id, _maxId);
  _maxId = 0;
  if(size<=(size_t)idMaxTmp) resize(min((size_t)1.5*(idMaxTmp+100),(size_t)(idMaxTmp+2000)));
}

const Vector3r& ForceContainer::getForceUnsynced (Body::id_t id) {
  return getForce(id);
}

const Vector3r& ForceContainer::getTorqueUnsynced(Body::id_t id) {
  return getForce(id);
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

void ForceContainer::addMaxId(Body::id_t id) {
  _maxId=id;
}

const Vector3r& ForceContainer::getPermForce(Body::id_t id) {
  ensureSize(id);
  return _permForce[id];
}

const Vector3r& ForceContainer::getPermTorque(Body::id_t id) {
  ensureSize(id);
  return _permTorque[id];
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
const Vector3r& ForceContainer::getMoveSingle(Body::id_t id) {
  ensureSize(id);
  return _move [id];
}

const Vector3r& ForceContainer::getRotSingle(Body::id_t id) {
  ensureSize(id);
  return _rot[id];
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
const bool& ForceContainer::getMoveRotUsed() const {return moveRotUsed;}
const bool& ForceContainer::getPermForceUsed() const {return permForceUsed;}
#endif

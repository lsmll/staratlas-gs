#include <core/scheduler.hh>

namespace wasmtm {

SchedulerQueue::SchedulerQueue(SchedulerQueue::Type type) {
    this->_type = type;
    this->init_queue();
}

void SchedulerQueue::push(nid_t nid, f64 p) {
    switch (this->_type)
    {
    case SchedulerQueue::FIFO:
        std::get<std::queue<nid_t>>(this->_Q).emplace(nid);
        break;
    case SchedulerQueue::FILO:
        std::get<std::stack<nid_t>>(this->_Q).emplace(nid);
        break;
    case SchedulerQueue::PRIO:
        std::get<std::priority_queue<std::pair<f64, nid_t>>>(this->_Q).emplace(p, nid);
        break;
    default:
        break;
    }
}

nid_t SchedulerQueue::pop() {
    nid_t nid = 0;
    switch (this->_type)
    {
    case SchedulerQueue::FIFO:
        nid = std::get<std::queue<nid_t>>(this->_Q).front();
        std::get<std::queue<nid_t>>(this->_Q).pop();
        break;
    case SchedulerQueue::FILO:
        nid = std::get<std::stack<nid_t>>(this->_Q).top();
        std::get<std::stack<nid_t>>(this->_Q).pop();
        break;
    case SchedulerQueue::PRIO:
        nid = std::get<std::priority_queue<std::pair<f64, nid_t>>>(this->_Q).top().second;
        std::get<std::priority_queue<std::pair<f64, nid_t>>>(this->_Q).pop();
        break;
    default:
        break;
    }
    return nid;
}

size_t SchedulerQueue::size() const {
    size_t sz = 0;
    switch (this->_type)
    {
    case SchedulerQueue::FIFO:
        sz = std::get<std::queue<nid_t>>(this->_Q).size();
        break;
    case SchedulerQueue::FILO:
        sz = std::get<std::stack<nid_t>>(this->_Q).size();
        break;
    case SchedulerQueue::PRIO:
        sz = std::get<std::priority_queue<std::pair<f64, nid_t>>>(this->_Q).size();
        break;
    default:
        break;
    }
    return sz;
}

bool SchedulerQueue::empty() const {
    bool em = true;
    switch (this->_type)
    {
    case SchedulerQueue::FIFO:
        em = std::get<std::queue<nid_t>>(this->_Q).empty();
        break;
    case SchedulerQueue::FILO:
        em = std::get<std::stack<nid_t>>(this->_Q).empty();
        break;
    case SchedulerQueue::PRIO:
        em = std::get<std::priority_queue<std::pair<f64, nid_t>>>(this->_Q).empty();
        break;
    default:
        break;
    }
    return em;
}

void SchedulerQueue::clear() {
    switch (this->_type)
    {
    case SchedulerQueue::FIFO:
        this->_Q = std::queue<nid_t>();
        break;
    case SchedulerQueue::FILO:
        this->_Q = std::stack<nid_t>();
        break;
    case SchedulerQueue::PRIO:
        this->_Q = std::priority_queue<std::pair<f64, nid_t>>();
        break;
    default:
        break;
    }
}

void SchedulerQueue::init_queue() {
    switch (this->_type)
    {
    case SchedulerQueue::FIFO:
        this->_Q = std::queue<nid_t>();
        break;
    case SchedulerQueue::FILO:
        this->_Q = std::stack<nid_t>();
        break;
    case SchedulerQueue::PRIO:
        this->_Q = std::priority_queue<std::pair<f64, nid_t>>();
        break;
    default:
        this->_type = SchedulerQueue::FIFO;
        this->_Q = std::queue<nid_t>();
        break;
    }
}

}
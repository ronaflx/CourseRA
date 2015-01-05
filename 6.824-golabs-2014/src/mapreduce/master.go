package mapreduce

import "container/list"
import "fmt"
import "strconv"
import "time"

type WorkerInfo struct {
	address string
}

// Clean up all workers by sending a Shutdown RPC to each one of them Collect
// the number of jobs each work has performed.
func (mr *MapReduce) KillWorkers() *list.List {
	l := list.New()
	for _, w := range mr.Workers {
		DPrintf("DoWork: shutdown %s\n", w.address)
		args := &ShutdownArgs{}
		var reply ShutdownReply
		ok := call(w.address, "Worker.Shutdown", args, &reply)
		if ok == false {
			fmt.Printf("DoWork: RPC %s shutdown error\n", w.address)
		} else {
			l.PushBack(reply.Njobs)
		}
	}
	return l
}

func requestWorker(mr *MapReduce, args *DoJobArgs, channel *chan int) {
	workerId := 0
	for {
		w := mr.Workers[strconv.Itoa(workerId)]
		var reply DoJobReply
		ok := call(w.address, "Worker.DoJob", args, &reply)
		if reply.OK && ok {
			*channel <- args.JobNumber
			break
		} else {
			workerId = (workerId + 1) % mr.nWorker
		}
	}
}

func (mr *MapReduce) RunMaster() *list.List {
	// Your code here
	// Every mapper has been registered.
	finishedMapper := make(chan int)
	finishedRecuder := make(chan int)
	go func() {
		for workerName := range mr.registerChannel {
			mr.Workers[strconv.Itoa(mr.nWorker)] = &WorkerInfo{workerName}
			mr.nWorker++
		}
	}()

	// Wait until at least one worker.
	for mr.nWorker == 0 {
		time.Sleep(time.Microsecond * 50)
	}

	// Send mapper job request to worker.
	for i := 0; i < mr.nMap; i++ {
		args := &DoJobArgs{}
		args.File = mr.file
		args.JobNumber = i
		args.Operation = Map
		args.NumOtherPhase = mr.nReduce

		go requestWorker(mr, args, &finishedMapper)
	}
	// Wait for all mappers to finish.
	for i := 0; i < mr.nMap; i++ {
		<-finishedMapper
	}

	// Send reducer job request to worker.
	for i := 0; i < mr.nReduce; i++ {
		args := &DoJobArgs{}
		args.File = mr.file
		args.JobNumber = i
		args.Operation = Reduce
		args.NumOtherPhase = mr.nMap
		go requestWorker(mr, args, &finishedRecuder)
	}
	// Wait for all reducers to finish.
	for i := 0; i < mr.nReduce; i++ {
		<-finishedRecuder
	}
	return mr.KillWorkers()
}

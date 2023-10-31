package clients

type RunRequest struct {
	BinaryPath string
}

type RunResponse struct {
	Success      bool
	Vd           uint64
	ErrorMessage string
}

type KillRequest struct {
	Vd uint64
}

type KillResponse struct {
	Success bool
}

type StatusRequest struct {
	Vd uint64
}

type VmStatus uint8

const (
	VmStatus_Running       VmStatus = 0
	VmStatus_Finished      VmStatus = 1
	VmStatus_Crashed       VmStatus = 2
	VmStatus_Killed        VmStatus = 3
	VmStatus_Timeout       VmStatus = 4
	VmStatus_InternalError VmStatus = 5
)

type StatusResponse struct {
	Success bool
	Status  VmStatus
}

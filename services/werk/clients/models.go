package clients

type RunRequest struct {
	BinaryPath    string
	SerialOutPath string
}

type RunResponse struct {
	Success      bool
	Vd           uint64
	ErrorMessage string
}

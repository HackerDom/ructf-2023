using Domain;
using Grpc.Core;
using Infrastructure.Database;

namespace API.Services;

public class AuthService : Auther.AutherBase
{
    private readonly JwtAuthManager _jwtAuthManager;
    private readonly IUserRepository _userRepository;

    public AuthService(IUserRepository userRepository,
        JwtAuthManager jwtAuthManager
    )
    {
        _userRepository = userRepository;
        _jwtAuthManager = jwtAuthManager;
    }

    public override async Task<RegisterReply> Register(
        RegisterRequest request,
        ServerCallContext context
    )
    {
        if (_userRepository.CheckUsernameExistsAsync(request.Username).Result)
            throw new RpcException(
                new Status(StatusCode.AlreadyExists, "This username already exists")
            );

        if (!Enum.TryParse(request.SpeciesType, out SpeciesType _))
            throw new RpcException(
                new Status(StatusCode.InvalidArgument, "Invalid species type")
            );

        await _userRepository.CreateAsync(request.Username, JwtAuthManager.GetPasswordHash(request.Password), request.SpeciesType);

        return await Task.FromResult(new RegisterReply { Success = true });
    }

    public override async Task<LoginReply> Login(LoginRequest request, ServerCallContext context)
    {
        var accessToken = await _jwtAuthManager.Authenticate(request.Username, request.Password);
        return await Task.FromResult(new LoginReply { AccessToken = accessToken });
    }
}
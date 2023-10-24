using Domain;
using Google.Protobuf;
using Grpc.Core;
using Infrastructure.Database;
using Microsoft.AspNetCore.Authorization;

namespace API.Services;

[Authorize]
public class AncestorService : AncestorServ.AncestorServBase
{
    private readonly IAncestorRepository _ancestorRepository;
    private readonly JwtAuthManager _jwtAuthManager;

    public AncestorService(JwtAuthManager jwtAuthManager,
        IAncestorRepository ancestorRepository)
    {
        _jwtAuthManager = jwtAuthManager;
        _ancestorRepository = ancestorRepository;
    }

    public override async Task<AncestorData> GetAncestor(
        GetAncestorRequest request,
        ServerCallContext context
    )
    {
        var isCacheUser = false;
        var user = await _jwtAuthManager.GetCacheUserByContext(context);
        if (user != null) isCacheUser = true;
        else user = await _jwtAuthManager.GetUserByContext(context);
        
        var requestId = ParseGuid(request.Id);

        var ancestor = await _ancestorRepository.GetByIdAsync(requestId);
        if (ancestor == null) throw new RpcException(new Status(StatusCode.NotFound, "Ancestor not found"));

        if (user.Species != ancestor.Species)
            throw new RpcException(new Status(StatusCode.PermissionDenied,
                "You are not allowed to access this ancestor"));

        var ancestorData = new AncestorData
        {
            Id = ByteString.CopyFrom(ancestor.Id.ToByteArray()),
            Name = ancestor.Name,
            Description = ancestor.Description,
            AncestorType = ancestor.Species.ToString()
        };

        var isAuth = user.Id == ancestor.OwnerId;
        if (isCacheUser)
        {
            Console.WriteLine(1);
            if (!user.DirectAncestors.Contains(ancestor.Id))
            {
                Console.WriteLine(2);
                user = await _jwtAuthManager.GetUserByContext(context);
                if (user.DirectAncestors.Contains(ancestor.Id))
                    isAuth = true;
            }
            else isAuth = true;
        }
        else if (user.DirectAncestors.Contains(ancestor.Id))
        {
            Console.WriteLine(3);
            isAuth = true;
        }

        if (isAuth) ancestorData.BurialPlace = ancestor.BurialPlace;
        
        return await Task.FromResult(ancestorData);
    }

    public override async Task<CreateAncestorResponse> CreateAncestor(
        AncestorData request,
        ServerCallContext context
    )
    {
        var user = await _jwtAuthManager.GetUserByContext(context);

        var requestId = ParseGuid(request.Id);

        if (_ancestorRepository.CheckExistsByIdAsync(requestId).Result)
            throw new RpcException(new Status(StatusCode.AlreadyExists, "Ancestor already exists"));

        if (!Enum.TryParse(request.AncestorType, out SpeciesType species))
            throw new RpcException(
                new Status(StatusCode.InvalidArgument, "Invalid species type")
            );
        if (user.Species != species)
            throw new RpcException(new Status(StatusCode.PermissionDenied,
                "You are not allowed to create this ancestor type"));

        var ancestor = new Ancestor
        {
            Id = user.Species == SpeciesType.GreatRuss ? Guid.NewGuid() : requestId,
            Name = request.Name,
            Description = request.Description,
            Species = species,
            BurialPlace = request.BurialPlace,
            OwnerId = user.Id
        };

        await _ancestorRepository.CreateAsync(ancestor);

        return new CreateAncestorResponse
        {
            Id = ByteString.CopyFrom(ancestor.Id.ToByteArray())
        };
    }

    public override async Task<AddDirectAncestorResponse> AddDirectAncestor(
        AddDirectAncestorRequest request,
        ServerCallContext context
    )
    {
        var user = await _jwtAuthManager.GetUserByContext(context);

        var ancestorId = ParseGuid(request.AncestorId);

        if (!_ancestorRepository.CheckExistsByIdAsync(ancestorId).Result)
            throw new RpcException(new Status(StatusCode.NotFound, "Ancestor not found"));

        if (user.Species != SpeciesType.FilthyLizard)
            throw new RpcException(new Status(StatusCode.PermissionDenied,
                "You are not allowed to add to ancestors"));

        if (user.DirectAncestors.Contains(ancestorId))
            throw new RpcException(new Status(StatusCode.AlreadyExists,
                "You are already add this ancestor"));

        await _ancestorRepository.AddDirectAncestorToUser(ancestorId, user.Id);

        return await Task.FromResult(new AddDirectAncestorResponse { Success = true });
    }

    private Guid ParseGuid(ByteString guidBytes)
    {
        try
        {
            return new Guid(guidBytes.ToByteArray());
        }
        catch
        {
            throw new RpcException(new Status(StatusCode.InvalidArgument, "Invalid ID"));
        }
    }
}
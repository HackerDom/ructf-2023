# Generated by the gRPC Python protocol compiler plugin. DO NOT EDIT!
"""Client and server classes corresponding to protobuf-defined services."""
import grpc

import client.auth.auth_pb2 as auth__pb2


class AutherStub(object):
    """Missing associated documentation comment in .proto file."""

    def __init__(self, channel):
        """Constructor.

        Args:
            channel: A grpc.Channel.
        """
        self.Register = channel.unary_unary(
            '/Auther/Register',
            request_serializer=auth__pb2.RegisterRequest.SerializeToString,
            response_deserializer=auth__pb2.RegisterReply.FromString,
        )
        self.Login = channel.unary_unary(
            '/Auther/Login',
            request_serializer=auth__pb2.LoginRequest.SerializeToString,
            response_deserializer=auth__pb2.LoginReply.FromString,
        )


class AutherServicer(object):
    """Missing associated documentation comment in .proto file."""

    def Register(self, request, context):
        """Missing associated documentation comment in .proto file."""
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def Login(self, request, context):
        """Missing associated documentation comment in .proto file."""
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')


def add_AutherServicer_to_server(servicer, server):
    rpc_method_handlers = {
        'Register': grpc.unary_unary_rpc_method_handler(
            servicer.Register,
            request_deserializer=auth__pb2.RegisterRequest.FromString,
            response_serializer=auth__pb2.RegisterReply.SerializeToString,
        ),
        'Login': grpc.unary_unary_rpc_method_handler(
            servicer.Login,
            request_deserializer=auth__pb2.LoginRequest.FromString,
            response_serializer=auth__pb2.LoginReply.SerializeToString,
        ),
    }
    generic_handler = grpc.method_handlers_generic_handler(
        'Auther', rpc_method_handlers)
    server.add_generic_rpc_handlers((generic_handler,))


# This class is part of an EXPERIMENTAL API.
class Auther(object):
    """Missing associated documentation comment in .proto file."""

    @staticmethod
    def Register(request,
                 target,
                 options=(),
                 channel_credentials=None,
                 call_credentials=None,
                 insecure=False,
                 compression=None,
                 wait_for_ready=None,
                 timeout=None,
                 metadata=None):
        return grpc.experimental.unary_unary(request, target, '/Auther/Register',
                                             auth__pb2.RegisterRequest.SerializeToString,
                                             auth__pb2.RegisterReply.FromString,
                                             options, channel_credentials,
                                             insecure, call_credentials, compression, wait_for_ready, timeout, metadata)

    @staticmethod
    def Login(request,
              target,
              options=(),
              channel_credentials=None,
              call_credentials=None,
              insecure=False,
              compression=None,
              wait_for_ready=None,
              timeout=None,
              metadata=None):
        return grpc.experimental.unary_unary(request, target, '/Auther/Login',
                                             auth__pb2.LoginRequest.SerializeToString,
                                             auth__pb2.LoginReply.FromString,
                                             options, channel_credentials,
                                             insecure, call_credentials, compression, wait_for_ready, timeout, metadata)
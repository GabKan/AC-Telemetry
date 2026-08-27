from fastapi import FastAPI, APIRouter

main_router = APIRouter()

from app.routers.telemetry import telemetry_router
main_router.include_router(telemetry_router)

from app.routers.session import session_router
main_router.include_router(session_router)

from app.routers.reference import reference_router
main_router.include_router(reference_router)


@main_router.get("/health")
def health_check():
    return {"status": "ok"}

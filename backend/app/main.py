from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware

from app.routers import main_router

# from app.routers import telemetry

app = FastAPI(title="AC Telementry Review")
app.include_router(main_router)

# app.include_router(telemetry.router)

app.add_middleware(
    CORSMiddleware,
    allow_origins=["http://localhost:5173"],
    allow_methods=["*"],
    allow_headers=["*"],
)

